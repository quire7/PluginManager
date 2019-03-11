#ifndef __ND_ZMQ_SERVICE_IMPLEMENT_H__
#define __ND_ZMQ_SERVICE_IMPLEMENT_H__
// ZMQServiceImpl.h �ӿ�ʵ��

#include <list>
using std::list;

#include <thread>
#include <string>
#include "zmq.h"
using namespace std;

class CriticalSection;
class ZMQService;

/// ZMQServiceʵ��
class ZMQServiceImpl
{
public:
    /// @param[in] service ���ã����ڻص�onMessage
    ZMQServiceImpl(ZMQService* service);
    ~ZMQServiceImpl();
    
    int start(const char* svrAddr);
    void stop();

    void sendMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);
    
private:
    /// �շ��¼��ɷ�ѭ��
    void dispatchLoop();

    /// ת����Ϣ
    void sendMessage();
    
    /// ������Ϣ
    void recvMessage();
    
    /// �ر�ZMQ���
    void closeHandle();
   

private:
    /// ��������״̬
    bool m_started;

    /// �����ַ
    string m_svrAddr;
    
    /// �շ��¼��ɷ�ѭ���߳�
    std::thread m_dispatchThread;

    /// ��������Ϣ�������( pair<clientId, respContent> );
	struct SendMsgInfo {
		int		length;
		char*	szBuf;
	};
    list<std::pair<SendMsgInfo, SendMsgInfo> > m_sendMsgList;

    /// ��������Ϣ���������
	CriticalSection* m_pSendMsgMutex;
    
private:
    /// ZMQ������
    void* m_zmqCTX;
    
    /// ����Զ�����ݾ��
    void* m_zmqTransData;
    
    /// ���գ��ܵ����¼����
    void* m_zmqPullEvent;
    
    /// ���ͣ��ܵ����¼����
    void* m_zmqPushEvent;
    
    /// ���ͣ��ܵ����¼������
	CriticalSection* m_pZmqPushMutex;
    
    /// ����Ϣ֡
    zmq_msg_t m_zmqFrameNULL;
    
    /// ���ܵ����¼����
    uint64_t m_seqPushEvent;
    
    /// ����
    ZMQService* const m_service;
};
#endif // !__ND_ZMQ_SERVICE_IMPLEMENT_H__
