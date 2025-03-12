#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <string>

using namespace std;
using namespace muduo;

ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

ChatService::ChatService() {
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    LOG_INFO << "do login service!";
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    LOG_INFO << "do reg service!";
}

MsgHandler ChatService::getHandler(int msgId) {
    auto it = _msgHandlerMap.find(msgId);  // 一次查找
    if (it == _msgHandlerMap.end()) {
        return [msgId](const TcpConnectionPtr&, json&, Timestamp) {
            LOG_ERROR << "msgid:" << msgId << " can not find handler!";
        };
    } else {
        return it->second;  // 直接返回迭代器指向的值
    }
}