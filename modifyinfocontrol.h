/*
func: 修改信息控制类。
author: zouyujie
date: 2025.2.2
*/
#ifndef MODIFYINFOCONTROL_H
#define MODIFYINFOCONTROL_H

#include <string>
using std::string;

class ModifyInfoControl
{
public:
    static ModifyInfoControl *getInstance();
    void modify_info(const string &buf);

private:
    ModifyInfoControl();
};

#endif // MODIFYINFOCONTROL_H
