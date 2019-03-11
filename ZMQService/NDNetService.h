#ifndef __ND_NET_SERVICE_H__
#define __ND_NET_SERVICE_H__

// ZMQService.h 接口层


#include "ZMQService.h"



class NDNetService : public ZMQService
{   
// 事件响应
public:
    /// （派生实现）处理接收到的请求消息
    /// @attention 实现侧应仅复制接收到的消息，转移到其他地方处理，否则会阻塞I/O线程）
    /// @param[in] remoteId    发送消息的远端地址
    /// @param[in] remoteIdLen 发送消息的远端地址长度
    /// @param[in] msgData     接收到的消息内容
    /// @param[in] msgLen      接收到的消息内容长度
    void onRecvMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);

public:
	NDNetService();
    ~NDNetService();
};
#endif // !__ND_NET_SERVICE_H__
