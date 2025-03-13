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
    _msgHandlerMap.insert({LOGIN_MSG, [this](const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
        this->login(conn, js, timestamp);
    }});
    _msgHandlerMap.insert({REG_MSG, [this](const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
        this->reg(conn, js, timestamp);
    }});
    _msgHandlerMap.insert({ONE_CHAT_MSG, [this](const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
        this->oneChat(conn, js, timestamp);
    }});
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    int id = js["id"].get<int>();
    string password = js["password"];
    json response;
    User user = _userModel.query(id);
    if (user.getPassword() == password) {
        //登录成功,但是用户已经登录
        if (user.getState() == "online") {
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";
            conn->send(response.dump());
        } else {
            //登录成功，记录用户连接状态
            {
                lock_guard <mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            //登录成功，记录修改用户状态信息 state offline=>online
            user.setState("online");
            _userModel.updateState(user);

            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            conn->send(response.dump());
        }
    } else {
        //登录失败
        response["msgid"] = id;
        response["errno"] = 1;
        response["errmsg"] = "用户名不存在或密码错误";
        conn->send((response.dump()));
    }
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setName(name);
    user.setPassword(password);
    if (_userModel.insert(user)) {
        //注册成功
        json response;
        response["msgid"] = REG_ACK_MSG;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } else {
        //注册失败
        json response;
        response["msgid"] = REG_ACK_MSG;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    int toid = js["toid"];
    unordered_map<int, TcpConnectionPtr>::iterator it;
    {
        lock_guard <mutex> lock(_connMutex);
        it = _userConnMap.find(toid);
    }
    if (it != _userConnMap.end()) {
        //to id在线，转发消息
        it->second->send(js.dump());
        return;
    } else {
        //to id离线，需要存储离线消息表
        
    }
}

MsgHandler ChatService::getHandler(int msgId) {
    auto it = _msgHandlerMap.find(msgId);  // 一次查找
    if (it == _msgHandlerMap.end()) {
        return [msgId](const TcpConnectionPtr &, json &, Timestamp) {
            LOG_ERROR << "msgid:" << msgId << " can not find handler!";
        };
    } else {
        return it->second;  // 直接返回迭代器指向的值
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn) {
    User user;
    {
        lock_guard <mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++) {
            if (it->second == conn) {
                user.setId(it->first);
                _userConnMap.erase(it->first);
                break;
            }
        }
    }
    //更新用户的状态信息
    user.setState("offline");
    _userModel.updateState(user);
}