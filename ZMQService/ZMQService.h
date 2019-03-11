#ifndef __ND_ZMQ_SERVICE_H__
#define __ND_ZMQ_SERVICE_H__

// ZMQService.h 接口层


#include <string>
using std::string;

class ZMQServiceImpl;


/// ZMQ ROUTER模式服务（请求应答服务）
/// 1. 收集远端数据，并转交业务处理（需要派生实现onRecvMessage）
/// 2. 转发业务的响应
class ZMQService
{
// 服务启停
public:
    /// 启动ZMQ收发请求消息服务
    /// @param[in] svrAddr 服务地址，形如：tcp://*:9999
    /// @return 0 成功；-1，已启动；其他值，其他错误
    int start(const char* svrAddr);
    
    /// 停止ZMQ收发请求消息服务
    void stop();

// 发送数据
public:
    /// （线程安全的）发送消息数据
    /// @param[in] remoteId    目标远端地址
    /// @param[in] msgData     待发送消息内容
    /// @param[in] msgLen      待发送消息内容长度
    void sendMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);
    
// 事件响应
public:
    /// （派生实现）处理接收到的请求消息
    /// @attention 实现侧应仅复制接收到的消息，转移到其他地方处理，否则会阻塞I/O线程）
    /// @param[in] remoteId    发送消息的远端地址
    /// @param[in] remoteIdLen 发送消息的远端地址长度
    /// @param[in] msgData     接收到的消息内容
    /// @param[in] msgLen      接收到的消息内容长度
    virtual void onRecvMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen) = 0;

public:
    ZMQService();
    virtual ~ZMQService();

private:
    ZMQServiceImpl* m_pImpl;
};
#endif // !__ND_ZMQ_SERVICE_H__
