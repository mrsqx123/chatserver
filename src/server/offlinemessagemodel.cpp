#include "offlinemessagemodel.hpp"
#include "db.hpp"

void OfflineMsgModel::insert(int userid, string msg) {
    char sql[1024] = {0};
    sprintf(sql, "insert into OfflineMessage values ('%d', '%s')", userid, msg.c_str());

    MySQL mysql;
    if (mysql.connect() && mysql.update(sql)) {}
}

void OfflineMsgModel::remove(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userid = %d", userid);

    MySQL mysql;
    if (mysql.connect() && mysql.update(sql)) {}
}

vector <string> OfflineMsgModel::query(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select message from OfflineMessage where userid = %d", userid);

    vector <string> v_msg;
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                v_msg.push_back(row[0]);
            }
        }
        mysql_free_result(res);
    }
    return v_msg;
}