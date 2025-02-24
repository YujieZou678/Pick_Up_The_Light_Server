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
#include <shared_mutex>
using std::shared_mutex;
using std::shared_lock;

#include "noncopyable.h"
#include "singleton.h"

struct Info {
    string ip;  //ip地址
    string userId;  //用户id
    int num;    //评估数据
};

class UserStatusEvaluator : private Noncopyable
{
    friend class Singleton<UserStatusEvaluator>;  //赋予单例类调用构造权限
public:
    void start();  //独立线程监测当前数据
    void add(int fd, const string &ip);  //增加数据
    void remove(int fd);  //移除数据
    void set_0(int fd);   //评估数据置0
    void add_userId(int fd, const string &userId);  //添加用户id
    string get_ip(int fd);  //根据fd去获取对应的ip
    string get_userId(int fd);  //根据fd去获取对应的userId

private:
    UserStatusEvaluator();

    shared_mutex m_mutex;
    unordered_map<int,Info> m_map;  //数据 fd : Info
};

#endif // USERSTATUSEVALUATOR_H
