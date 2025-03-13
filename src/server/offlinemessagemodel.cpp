#include "offlinemessagemodel.hpp"
#include "db.hpp"

void OfflineMsgModel::insert(int userid, string msg) {
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessagemodel values ('%d', '%s')", userid, msg.c_str());

    MySQL mysql;
    if (mysql.connect() && mysql.update(sql)) {}
}

void OfflineMsgModel::remove(int userid) {
    char sql[1024] = {0};
}

list <string> OfflineMsgModel::query(int userid) {
    list <string> list;
    return list;
}