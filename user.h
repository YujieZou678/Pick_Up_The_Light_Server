/*
func: 用户实体类。
author: zouyujie
date: 2025.1.21
*/
#ifndef USER_H
#define USER_H

#include "userinterface.h"

class User : public UserInterface
{
public:
    User(const string &id, const string &pw, const string &nickName);
    ~User() {}

    json getInfo() override;

    string getPw() const;
    string getNickName() const;

private:
    string m_pw;  //密码
    string m_nickName;  //呢称
};

#endif // USER_H
