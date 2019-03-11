#ifndef __ND_NET_SERVICE_H__
#define __ND_NET_SERVICE_H__

// ZMQService.h �ӿڲ�


#include "ZMQService.h"



class NDNetService : public ZMQService
{   
// �¼���Ӧ
public:
    /// ������ʵ�֣�������յ���������Ϣ
    /// @attention ʵ�ֲ�Ӧ�����ƽ��յ�����Ϣ��ת�Ƶ������ط��������������I/O�̣߳�
    /// @param[in] remoteId    ������Ϣ��Զ�˵�ַ
    /// @param[in] remoteIdLen ������Ϣ��Զ�˵�ַ����
    /// @param[in] msgData     ���յ�����Ϣ����
    /// @param[in] msgLen      ���յ�����Ϣ���ݳ���
    void onRecvMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);

public:
	NDNetService();
    ~NDNetService();
};
#endif // !__ND_NET_SERVICE_H__
