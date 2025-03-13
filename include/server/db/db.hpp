#ifndef DB_HPP
#define DB_HPP

#include "mysql/mysql.h"
#include <string>
#include <muduo/base/Logging.h>

using namespace std;

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat";

// 数据库操作类
class MySQL {
public:
    // 初始化数据库连接
    MySQL() {
        _conn = mysql_init(nullptr);
    }

    // 释放数据库连接资源
    ~MySQL() {
        if (_conn != nullptr)
            mysql_close(_conn);
    }

    // 连接数据库
    bool connect() {
        MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                      password.c_str(), dbname.c_str(), 3306, nullptr, 0);
        if (p != nullptr) {
            //C和C++代码默认的编码字符是ASCII，设置MYSQL编码为GBK 如果不设置字符编码，从数据库读取或写入数据时可能会出现乱码。
            mysql_query(_conn, "set names gbk");
            LOG_INFO << "connect mysql success!";
        }
        return p;
    }

    // 更新操作
    bool update(string sql) {
        if (mysql_query(_conn, sql.c_str())) {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                     << sql << "更新失败!";
            return false;
        }
        return true;
    }

    // 查询操作
    MYSQL_RES *query(string sql) {
        if (mysql_query(_conn, sql.c_str())) {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                     << sql << "查询失败!";
            return nullptr;
        }
        return mysql_use_result(_conn);
    }

    // 获取连接
    MYSQL *getConnection() {
        return _conn;
    }

private:
    MYSQL *_conn;
};

#endif