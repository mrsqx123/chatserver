#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include "group.hpp"


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
    _msgHandlerMap.insert({LOGINOUT_MSG, [this](const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
        this->loginout(conn, js, timestamp);
    }});
    _msgHandlerMap.insert({ONE_CHAT_MSG, [this](const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
        this->oneChat(conn, js, timestamp);
    }});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, [this](const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
        this->addFriend(conn, js, timestamp);
    }});

    //连接redis服务器
    if (_redis.connect()) {
        //注册一个回调函数
        _redis.init_notify_handler(bind(&ChatService::handleSubscribeMsg, this, _1, _2));
    }
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    int id = js["id"].get<int>();
    string password = js["password"];
    json response;
    User user = _userModel.query(id);
    if (user.getPassword() == password) {
        //登录成功,但是用户已经登录
        if (user.getState() == "online") {
            response["msgid"] = LOGIN_ACK_MSG;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";
            conn->send(response.dump());
        } else {
            //登录成功，记录用户连接状态
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            //id用户登陆成功后，向redis订阅channel(id)
            _redis.subscribe(id);

            //登录成功，记录修改用户状态信息 state offline=>online
            user.setState("online");
            _userModel.updateState(user);

            response["msgid"] = LOGIN_ACK_MSG;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            //查询当前用户是否有离线消息
            vector<string> v_msg = _offlineMsgModel.query(id);
            if (!v_msg.empty()) {
                response["offlinemsg"] = v_msg;
                _offlineMsgModel.remove(id);
            }
            //查询该用户的好友信息并返回
            vector<User> friUser = _friendModel.query(id);
            if (!friUser.empty()) {
                vector<string> friString;
                for (User &user: friUser) {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    friString.push_back(js.dump());
                }
                response["friends"] = friString;
            }

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

void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end()) {
            _userConnMap.erase(it);
        }
    }

    _redis.unsubscribe(userid);

    User user;
    user.setId(userid);
    user.setState("offline");
    _userModel.updateState(user);
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    _friendModel.insert(userid, friendid);
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp timestamp) {
    int toid = js["toid"].get<int>();
    unordered_map<int, TcpConnectionPtr>::iterator it;
    {
        lock_guard<mutex> lock(_connMutex);
        it = _userConnMap.find(toid);
        if (it != _userConnMap.end()) {
            //to id在线，转发消息
            it->second->send(js.dump());
            return;
        }
    }

    //查询id是否在线
    User user = _userModel.query(toid);
    if (user.getState() == "online") {
        _redis.publish(toid, js.dump());
        return;
    }

    //to id离线，需要存储离线消息表
    OfflineMsgModel _offlineMsgModel;
    _offlineMsgModel.insert(js["toid"], js["msg"]);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group)) {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    for (int id: useridVec) {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end()) {
            // 转发群消息
            it->second->send(js.dump());
        } else {
            // 查询toid是否在线
            User user = _userModel.query(id);
            if (user.getState() == "online") {
                _redis.publish(id, js.dump());
            } else {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
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
        lock_guard<mutex> lock(_connMutex);
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

void ChatService::handleSignal(int sig) {
    //把online状态的用户设置成offline
    _userModel.resetState();
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleSubscribeMsg(int userid, string msg) {
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end()) {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}