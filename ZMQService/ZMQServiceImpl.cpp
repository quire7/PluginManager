// ZMQService.cpp 接口实现
#include "ZMQServiceImpl.h"

#include "CriticalSection.h"
#include "CFunctionImpl.h"
#include "NDPluginManagerGlobal.h"

#include "ZMQService.h"




/// 线程间管道信号名称（如果启动ZMQService，必须修改使用不同的名称！！！）
#define EVENT_NOTIFIER_NAME  "inproc://zmq_service_event_notifier"


///////////////////////////////////////////////////////////////////////////////
ZMQServiceImpl::ZMQServiceImpl(ZMQService* service)
    : m_service(service)
    , m_zmqCTX(NULL)
    , m_zmqTransData(NULL)
    , m_zmqPullEvent(NULL)
    , m_zmqPushEvent(NULL)
    , m_seqPushEvent(0)
	, m_started(false)
{
    static char zero = 0;
	zmq_msg_init_size(&m_zmqFrameNULL, 0);

	m_pSendMsgMutex = new CriticalSection();
	m_pZmqPushMutex	= new CriticalSection();
}

ZMQServiceImpl::~ZMQServiceImpl()
{
    if (m_started)
        stop();
    
    zmq_msg_close(&m_zmqFrameNULL);

	if (NULL != m_pSendMsgMutex) {
		delete m_pSendMsgMutex;
		m_pSendMsgMutex = NULL;
	}
	if (NULL != m_pZmqPushMutex) {
		delete m_pZmqPushMutex;
		m_pZmqPushMutex = NULL;
	}
}

int ZMQServiceImpl::start(const char* svrAddr)
{
    CPMLog_Trace("zmqServer=%s", svrAddr);

    m_svrAddr = svrAddr; // save for re-start    
    if (m_started) {
		CPMLog_Error("error, zmq server is running");
        return -1;
    }
    
    m_zmqCTX = zmq_ctx_new();

    //= 启动接收数据、接收外发数据事件服务
    
    m_zmqTransData = zmq_socket(m_zmqCTX, ZMQ_ROUTER);
    m_zmqPullEvent = zmq_socket(m_zmqCTX, ZMQ_PULL);
	m_zmqPushEvent = zmq_socket(m_zmqCTX, ZMQ_PUSH);
    
    int ret = zmq_bind(m_zmqTransData, svrAddr);
    if (ret) {
		CPMLog_Error("fail, zmq server addr=%s, ret=%d, errno=%d", svrAddr, ret, errno);
        closeHandle();
        return -2;
    }
    
    ret = zmq_bind(m_zmqPullEvent, EVENT_NOTIFIER_NAME);
    if (ret) {
		CPMLog_Error("fail, create evPull=%s, ret=%d, errno=%d", EVENT_NOTIFIER_NAME, ret, errno);
        closeHandle();
        return -3;
    }

	 // 连接到事件监听通道
	 ret = zmq_connect(m_zmqPushEvent, EVENT_NOTIFIER_NAME);
	 if (ret) {
		 CPMLog_Error("fail, create evPush=%s, ret=%d, errno=%d", EVENT_NOTIFIER_NAME, ret, errno);
		 closeHandle();
		 return -4;
	 }

    m_started = true;
    m_dispatchThread = std::thread(&ZMQServiceImpl::dispatchLoop, this);
        
	CPMLog_Info("start zmqServer=%s, success", svrAddr);
    return 0;
}

void ZMQServiceImpl::stop()
{
	CPMLog_Trace("zmqServer=%s", m_svrAddr.c_str());

    if (!m_started) {
		CPMLog_Warn("service is stopped");
        return;
    }

    m_started = false;

    {
        // 发一个虚假消息，触发poll
		GuardLock lock(m_pZmqPushMutex);
        zmq_send(m_zmqPushEvent, &m_seqPushEvent, sizeof(uint64_t), 0);
        m_seqPushEvent++;
    }
    m_dispatchThread.join();
    
    closeHandle();
}

void ZMQServiceImpl::closeHandle()
{
	GuardLock lock(m_pZmqPushMutex);

    if (m_zmqTransData) zmq_close(m_zmqTransData);
    if (m_zmqPullEvent) zmq_close(m_zmqPullEvent);
    if (m_zmqPushEvent) zmq_close(m_zmqPushEvent);
    if (m_zmqCTX)       zmq_ctx_destroy(m_zmqCTX);
  
    m_zmqTransData = NULL;
    m_zmqPullEvent = NULL;
    m_zmqPushEvent = NULL;
    m_zmqCTX       = NULL;
}

void ZMQServiceImpl::sendMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen)
{
	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();
    //= 保存待发送消息到队列
    {
		//copy and save data;
		SendMsgInfo remoteIdInfo;
		remoteIdInfo.length = remoteIdLen;
		remoteIdInfo.szBuf = (char*)pNDPluginManagerGlobal->malloc( remoteIdInfo.length + 1 ) ;
		memcpy( remoteIdInfo.szBuf, remoteId, remoteIdInfo.length );
		remoteIdInfo.szBuf[remoteIdInfo.length] = '\0';

		SendMsgInfo msgDataInfo;
		msgDataInfo.length = msgLen;
		msgDataInfo.szBuf = (char*)pNDPluginManagerGlobal->malloc( msgLen + 1 );
		memcpy(msgDataInfo.szBuf, msgData, msgLen );
		msgDataInfo.szBuf[msgLen] = '\0';

		GuardLock lock(m_pSendMsgMutex);
        m_sendMsgList.push_back( std::make_pair( remoteIdInfo, msgDataInfo ) );
    }

    //= 通知收发线程，需要转发消息
    {
		GuardLock lock(m_pZmqPushMutex);
        zmq_send(m_zmqPushEvent, &m_seqPushEvent, sizeof(uint64_t), 0);
        m_seqPushEvent++;
    }
}

void ZMQServiceImpl::sendMessage()
{
	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();

    std::pair<SendMsgInfo, SendMsgInfo> msg;
    {
		GuardLock lock(m_pSendMsgMutex);

        if (m_sendMsgList.empty())
            return;

        msg = m_sendMsgList.front();
        m_sendMsgList.pop_front();
    }
    
    zmq_msg_t frameId, frameMsg;

    // 下一次poll才会真正发送数据，data必须持续有效
    //zmq_msg_init_data(&frameId, (void*)msg.first.data(), msg.first.size(), NULL, NULL);
    //zmq_msg_init_data(&frameMsg, (void*)msg.second.data(), msg.second.size(), NULL, NULL);
    //zmq_msg_init_data(&frameMsg, (void*)message.data(), message.size(), NULL, NULL);
    
    zmq_msg_init_size(&frameId, msg.first.length);
    zmq_msg_init_size(&frameMsg, msg.second.length);

    memcpy(zmq_msg_data(&frameId), msg.first.szBuf, msg.first.length);
    memcpy(zmq_msg_data(&frameMsg), msg.second.szBuf, msg.second.length);

    zmq_msg_send(&frameId, m_zmqTransData, ZMQ_SNDMORE);
    zmq_msg_send(&m_zmqFrameNULL, m_zmqTransData, ZMQ_SNDMORE);
    zmq_msg_send(&frameMsg, m_zmqTransData, ZMQ_DONTWAIT);
    
    zmq_msg_close(&frameId);
    zmq_msg_close(&frameMsg);

	pNDPluginManagerGlobal->free(msg.first.szBuf);
	pNDPluginManagerGlobal->free(msg.second.szBuf);
}

void ZMQServiceImpl::recvMessage()
{
    //= [clientId][空][请求数据];
    
    zmq_msg_t clientId, frameNULL, message;
    
    zmq_msg_init(&clientId);
    zmq_msg_init(&frameNULL);
    zmq_msg_init(&message);

    zmq_msg_recv(&clientId, m_zmqTransData, 0);
    zmq_msg_recv(&frameNULL, m_zmqTransData, 0);
    zmq_msg_recv(&message, m_zmqTransData, 0);

    zmq_msg_copy(&m_zmqFrameNULL, &frameNULL);

	const char* msg = (const char*)zmq_msg_data(&frameNULL);
	int len = (int)zmq_msg_size(&frameNULL);

    // 转发请求;
    m_service->onRecvMessage(
        (const char*)zmq_msg_data(&clientId), (int)zmq_msg_size(&clientId),
		(const char*)zmq_msg_data(&message), (int)zmq_msg_size(&message));
    
    zmq_msg_close(&clientId);
    zmq_msg_close(&frameNULL);
    zmq_msg_close(&message);
}

void ZMQServiceImpl::dispatchLoop()
{
    bool occurException = false;
    while (m_started)
    {
        if (occurException) {
			CPMLog_Warn("re-start after 1 second, zmqServer=%s", m_svrAddr.c_str());

            std::this_thread::sleep_for(std::chrono::seconds(1));
            occurException = (0 != start(m_svrAddr.c_str()));
            continue;
        }

        zmq_pollitem_t items[] = {
            { m_zmqTransData, 0, ZMQ_POLLIN, 0 },
            { m_zmqPullEvent, 0, ZMQ_POLLIN, 0 },
        };
        
        int ret = zmq_poll(items, 2, -1);
        if (ret == -1) {
			CPMLog_Warn("check a exception, zmq_poll ret=%d, err=%d", ret, errno);
            occurException = true;
            continue;
        }

        // 远程调用请求;
        if (items[0].revents & ZMQ_POLLIN)
			recvMessage();
        
        // 转发待发送数据;
		if (items[1].revents & ZMQ_POLLIN) {
            uint64_t evSequence;
            zmq_recv(m_zmqPullEvent, &evSequence, sizeof(uint64_t), 0);
            sendMessage();
        }
    }

	CPMLog_Info("service(%s) is stopped, exit dispath loop", m_svrAddr.c_str());
}