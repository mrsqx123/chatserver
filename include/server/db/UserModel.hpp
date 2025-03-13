#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "user.hpp"


class UserModel {
public:
    //user表增加
    bool insert(User &user);

    //根据id查找User
    User query(int id);

    //更新user信息
    bool updateState(User user);

private:

};

#endif