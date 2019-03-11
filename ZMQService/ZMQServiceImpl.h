#ifndef __ND_ZMQ_SERVICE_IMPLEMENT_H__
#define __ND_ZMQ_SERVICE_IMPLEMENT_H__
// ZMQServiceImpl.h 接口实现

#include <list>
using std::list;

#include <thread>
#include <string>
#include "zmq.h"
using namespace std;

class CriticalSection;
class ZMQService;

/// ZMQService实现
class ZMQServiceImpl
{
public:
    /// @param[in] service 引用，用于回调onMessage
    ZMQServiceImpl(ZMQService* service);
    ~ZMQServiceImpl();
    
    int start(const char* svrAddr);
    void stop();

    void sendMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);
    
private:
    /// 收发事件派发循环
    void dispatchLoop();

    /// 转发消息
    void sendMessage();
    
    /// 接收消息
    void recvMessage();
    
    /// 关闭ZMQ句柄
    void closeHandle();
   

private:
    /// 服务启动状态
    bool m_started;

    /// 服务地址
    string m_svrAddr;
    
    /// 收发事件派发循环线程
    std::thread m_dispatchThread;

    /// 待发送消息缓存队列( pair<clientId, respContent> );
	struct SendMsgInfo {
		int		length;
		char*	szBuf;
	};
    list<std::pair<SendMsgInfo, SendMsgInfo> > m_sendMsgList;

    /// 待发送消息缓存队列锁
	CriticalSection* m_pSendMsgMutex;
    
private:
    /// ZMQ上下文
    void* m_zmqCTX;
    
    /// 接收远端数据句柄
    void* m_zmqTransData;
    
    /// 接收（管道）事件句柄
    void* m_zmqPullEvent;
    
    /// 发送（管道）事件句柄
    void* m_zmqPushEvent;
    
    /// 发送（管道）事件句柄锁
	CriticalSection* m_pZmqPushMutex;
    
    /// 空消息帧
    zmq_msg_t m_zmqFrameNULL;
    
    /// （管道）事件序号
    uint64_t m_seqPushEvent;
    
    /// 引用
    ZMQService* const m_service;
};
#endif // !__ND_ZMQ_SERVICE_IMPLEMENT_H__
