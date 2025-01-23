/*
func: 初始化控制类，负责登陆与注册。
author: zouyujie
date: 2025.1.21
*/
#ifndef INITCONTROL_H
#define INITCONTROL_H

#include <string>
using std::string;

class InitControl
{
public:
    static InitControl *getInstance();
    bool do_register(const string &jsonMsg);
    bool do_login(const string &jsonMsg);

private:
    InitControl();
};

#endif // INITCONTROL_H
