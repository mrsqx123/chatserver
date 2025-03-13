#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <string>
#include <list>

using namespace std;

class OfflineMsgModel {
public:
    //存储用户的离线需求
    void insert(int userid, string msg);

    //删除用户的离线消息
    void remove(int userid);

    //查询用户的离线消息
    list<string> query(int userid);
};

#endif