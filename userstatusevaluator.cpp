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

UserStatusEvaluator *UserStatusEvaluator::m_instance = nullptr;

UserStatusEvaluator::UserStatusEvaluator()
{
}

UserStatusEvaluator *UserStatusEvaluator::getInstance()
{
    if (m_instance == nullptr) {
        m_instance = new UserStatusEvaluator();
    }
    return m_instance;
}

void UserStatusEvaluator::start()
{
    thread t([this](){  //捕获当前对象
        /* 循环检测 */
        std::cout << "the heartbeat check thread start..." << std::endl;
        unordered_map<int,pair<string,int>>::iterator it;
        /* 每3秒轮询检测一次心跳包情况 */
        while (1) {
            it = m_map.begin();
            for (it; it!=m_map.end(); it++) {
                pair<int,pair<string,int>> data = *it;
                if (data.second.second == 3) {  //连续3次检测没有心跳包，则判定客户端断开
                    std::cout << "UserStatusEvaluator：The client " << data.second.first
                              << " has been offline..." << std::endl;
                    int fd = data.first;
                    /* 心跳 */
                    unique_lock<mutex> lk(m_mutex);
                    m_map.erase(it++);
                    lk.unlock();
                    /* epoll */
                    EpollOperator::getInstance()->deleteFd(fd);
                    /* close */
                    close(fd);
                } else if (data.second.second < 3 && data.second.second >= 0) {
                    data.second.second += 1;
                    ++it;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
//    t.join();
}

void UserStatusEvaluator::add(int fd, string &ip)
{
    unique_lock<mutex> lk(m_mutex);
    m_map.insert(make_pair(fd, make_pair(ip, 0)));
}

void UserStatusEvaluator::remove(int fd)
{
    unique_lock<mutex> lk(m_mutex);
    m_map.erase(m_map.find(fd));
}


