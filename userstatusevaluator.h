/*
func: 用户连接状态评估者(处理心跳包)。
author: zouyujie
date: 2025.1.6
*/
#ifndef USERSTATUSEVALUATOR_H
#define USERSTATUSEVALUATOR_H

#include <unordered_map>
using std::unordered_map;
using std::pair;
#include <string>
using std::string;
#include <mutex>
using std::mutex;
using std::unique_lock;

class UserStatusEvaluator
{
public:
    static UserStatusEvaluator *getInstance();
    void start();  //独立线程监测当前数据
    void add(int fd, string &ip);    //增加数据
    void remove(int fd); //移除数据

private:
    UserStatusEvaluator();
    static UserStatusEvaluator *m_instance;

    mutex m_mutex;
    unordered_map<int,pair<string,int>> m_map;  //数据 fd : (ip : 0-3)
};

#endif // USERSTATUSEVALUATOR_H
