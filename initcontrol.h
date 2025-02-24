/*
func: 初始化控制类，负责登陆与注册。
author: zouyujie
date: 2025.1.21
*/
#ifndef INITCONTROL_H
#define INITCONTROL_H

#include <string>
using std::string;

#include "noncopyable.h"
#include "singleton.h"

class InitControl : private Noncopyable
{
    friend class Singleton<InitControl>;  //赋予单例类调用构造权限
public:
    bool do_register(const string &strMsg);
    bool do_login(int fd, const string &strMsg);

private:
    InitControl();
};

#endif // INITCONTROL_H
