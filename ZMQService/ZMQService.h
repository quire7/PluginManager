#ifndef __ND_ZMQ_SERVICE_H__
#define __ND_ZMQ_SERVICE_H__

// ZMQService.h �ӿڲ�


#include <string>
using std::string;

class ZMQServiceImpl;


/// ZMQ ROUTERģʽ��������Ӧ�����
/// 1. �ռ�Զ�����ݣ���ת��ҵ������Ҫ����ʵ��onRecvMessage��
/// 2. ת��ҵ�����Ӧ
class ZMQService
{
// ������ͣ
public:
    /// ����ZMQ�շ�������Ϣ����
    /// @param[in] svrAddr �����ַ�����磺tcp://*:9999
    /// @return 0 �ɹ���-1��������������ֵ����������
    int start(const char* svrAddr);
    
    /// ֹͣZMQ�շ�������Ϣ����
    void stop();

// ��������
public:
    /// ���̰߳�ȫ�ģ�������Ϣ����
    /// @param[in] remoteId    Ŀ��Զ�˵�ַ
    /// @param[in] msgData     ��������Ϣ����
    /// @param[in] msgLen      ��������Ϣ���ݳ���
    void sendMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);
    
// �¼���Ӧ
public:
    /// ������ʵ�֣�������յ���������Ϣ
    /// @attention ʵ�ֲ�Ӧ�����ƽ��յ�����Ϣ��ת�Ƶ������ط��������������I/O�̣߳�
    /// @param[in] remoteId    ������Ϣ��Զ�˵�ַ
    /// @param[in] remoteIdLen ������Ϣ��Զ�˵�ַ����
    /// @param[in] msgData     ���յ�����Ϣ����
    /// @param[in] msgLen      ���յ�����Ϣ���ݳ���
    virtual void onRecvMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen) = 0;

public:
    ZMQService();
    virtual ~ZMQService();

private:
    ZMQServiceImpl* m_pImpl;
};
#endif // !__ND_ZMQ_SERVICE_H__
