#ifndef PUBLIC_H
#define PUBLIC_H
/*
 * server和client的实现函数
 */
enum MsgType {
    LOGIN_MSG = 1,   //登录消息
    LOGIN_ACK_MSG,  //登录相应消息
    REG_MSG,         //注册消息
    REG_ACK_MSG,    //注册相应消息
    LOGINOUT_MSG,   //注销消息
    ONE_CHAT_MSG,   //聊天消息
    ADD_FRIEND_MSG, //添加好友信息

    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG, // 加入群组
    GROUP_CHAT_MSG, // 群聊天

};

#endif