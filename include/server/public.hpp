#ifndef PUBLIC_H
#define PUBLIC_H
/*
 * server和client的实现函数
 *
 */
enum MsgType {
    LOGIN_MSG = 1,   //登录消息
    LOGIN_MSG_ACK,  //登录相应消息
    REG_MSG,         //注册消息
    REG_ACK_MSG,    //注册相应消息
    ONE_CHAT_MSG,   //聊天消息
};

#endif