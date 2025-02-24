/*
func: 修改信息控制类。
author: zouyujie
date: 2025.2.2
*/
#ifndef MODIFYINFOCONTROL_H
#define MODIFYINFOCONTROL_H

#include <string>
using std::string;

#include "noncopyable.h"
#include "singleton.h"

class ModifyInfoControl : private Noncopyable
{
    friend class Singleton<ModifyInfoControl>;  //赋予单例类调用构造权限
public:
    void modify_info(int fd, const string &buf);

private:
    ModifyInfoControl();
};

#endif // MODIFYINFOCONTROL_H
