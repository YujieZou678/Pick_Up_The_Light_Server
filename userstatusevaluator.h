/*
func: 用户连接状态评估者。
author: zouyujie
date: 2025.1.6
*/
#ifndef USERSTATUSEVALUATOR_H
#define USERSTATUSEVALUATOR_H

#include <map>
using std::map;
using std::pair;
#include <string>
using std::string;

class UserStatusEvaluator
{
public:
    UserStatusEvaluator();

private:
    map<int,pair<string,int>> heart_check_map; //心跳检测map
};

#endif // USERSTATUSEVALUATOR_H
