#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <functional>
#include <unordered_map>
#include "muduo/net/TcpConnection.h"
#include "json.hpp"
#include "UserModel.hpp"
#include <mutex>

using namespace std;
using namespace muduo;
using namespace muduo::net;

using json = nlohmann::json;

// 表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

class ChatService {
public:
    static ChatService *instance();

    //处理登录
    void login(const TcpConnectionPtr &conn, json &js, Timestamp timestamp);

    //处理注册
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp timestamp);

    //一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp timestamp);

    //获取消息对应的处理函数
    MsgHandler getHandler(int msgId);

    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

private:
    ChatService();

    //存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;

    //存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    //定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;

    //数据操作类
    UserModel _userModel;
};

#endif