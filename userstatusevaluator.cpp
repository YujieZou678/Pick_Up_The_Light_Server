/*
func: 用户连接状态评估者的实现。
author: zouyujie
date: 2025.1.6
*/
#include "userstatusevaluator.h"

#include <thread>
using std::thread;
#include <iostream>

#include "epolloperator.h"

UserStatusEvaluator::UserStatusEvaluator()
{
}

UserStatusEvaluator *UserStatusEvaluator::getInstance()
{
    static UserStatusEvaluator instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

void UserStatusEvaluator::start()
{
    thread t([this](){  //捕获当前对象
        /* 循环检测 */
        std::cout << "the heartbeat check thread start..." << std::endl;
        unordered_map<int,Info>::iterator it;
        /* 每3秒轮询检测一次心跳包情况 */
        while (1) {
            shared_lock<shared_mutex> lk(m_mutex);  //加读写锁
            auto map_copy = m_map;  //拷贝，遍历只读
            it = map_copy.begin();
            for (it; it!=map_copy.end(); it++) {
                pair<int,Info> data = *it;
                int fd = data.first;
                if (data.second.num == 3) {  //连续3次检测没有心跳包，则判定客户端断开
                    std::cerr << "UserStatusEvaluator：The client " << data.second.ip
                              << " has been offline..." << std::endl;
                    /* 心跳 */
                    m_map.erase(fd);
                    /* epoll */
                    EpollOperator::getInstance()->deleteFd(fd);
                    /* close */
                    close(fd);
                } else if (data.second.num < 3 && data.second.num >= 0) {
                    m_map.at(fd).num += 1;
                }
            }
            lk.unlock();  //解锁

            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
    t.detach();
}

void UserStatusEvaluator::add(int fd, string &ip)
{
    Info info;
    info.ip = ip;
    info.num = 0;
    shared_lock<shared_mutex> lk(m_mutex);
    m_map.insert(std::make_pair(fd, info));
}

void UserStatusEvaluator::remove(int fd)
{
    shared_lock<shared_mutex> lk(m_mutex);
    m_map.erase(fd);
}

void UserStatusEvaluator::set_0(int fd)
{
    shared_lock<shared_mutex> lk(m_mutex);
    m_map.at(fd).num = 0;
}

string UserStatusEvaluator::get_ip(int fd)
{
    shared_lock<shared_mutex> lk(m_mutex);
    return m_map.at(fd).ip;
}


