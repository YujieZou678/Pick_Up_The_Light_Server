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
                    Singleton<EpollOperator>::getInstance()->deleteFd(fd);
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

void UserStatusEvaluator::start_boost()
{
    thread t([this](){  //捕获当前对象
        /* 循环检测 */
        std::cout << "the heartbeat check thread start..." << std::endl;
        unordered_map<shared_ptr<tcp::socket>,Info>::iterator it;
        /* 每3秒轮询检测一次心跳包情况 */
        while (1) {
            shared_lock<shared_mutex> lk(m_mutex);  //加读写锁
            auto map_copy = m_map_boost;  //拷贝，遍历只读
            it = map_copy.begin();
            for (it; it!=map_copy.end(); it++) {
                pair<shared_ptr<tcp::socket>,Info> data = *it;
                shared_ptr<tcp::socket> socket_ptr = data.first;
                if (data.second.num == 3) {  //连续3次检测没有心跳包，则判定客户端断开
                    std::cerr << "UserStatusEvaluator：The client " << data.second.ip
                              << "has been offline..." << std::endl;
                    /* 心跳 */
                    m_map_boost.erase(socket_ptr);
                    /* close */
                    socket_ptr.get()->close();
                } else if (data.second.num < 3 && data.second.num >= 0) {
                    m_map_boost.at(socket_ptr).num += 1;
                }
            }
            lk.unlock();  //解锁

            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
    t.detach();
}

void UserStatusEvaluator::add(int fd, const string &ip)
{
    Info info;
    info.ip = ip;
    info.num = 0;
    shared_lock<shared_mutex> lk(m_mutex);
    m_map.insert(std::make_pair(fd, info));
}

void UserStatusEvaluator::add(shared_ptr<tcp::socket> socket_ptr)
{
    Info info;
    info.ip = "";  //不需要
    info.num = 0;
    shared_lock<shared_mutex> lk(m_mutex);
    m_map_boost.insert(std::make_pair(socket_ptr, info));
}

void UserStatusEvaluator::remove(int fd)
{
    shared_lock<shared_mutex> lk(m_mutex);
    m_map.erase(fd);
}

void UserStatusEvaluator::remove(shared_ptr<tcp::socket> socket_ptr)
{
    shared_lock<shared_mutex> lk(m_mutex);
    m_map_boost.erase(socket_ptr);
}

void UserStatusEvaluator::set_0(int fd)
{
    shared_lock<shared_mutex> lk(m_mutex);
    m_map.at(fd).num = 0;
}

void UserStatusEvaluator::set_0(shared_ptr<tcp::socket> socket_ptr)
{
    shared_lock<shared_mutex> lk(m_mutex);
    m_map_boost.at(socket_ptr).num = 0;
}

void UserStatusEvaluator::add_userId(int fd, const string &userId)
{
    shared_lock<shared_mutex> lk(m_mutex);
    m_map.at(fd).userId = userId;
}

void UserStatusEvaluator::add_userId(shared_ptr<tcp::socket> socket_ptr, const string &userId) {
    shared_lock<shared_mutex> lk(m_mutex);
    m_map_boost.at(socket_ptr).userId = userId;
}

string UserStatusEvaluator::get_ip(int fd)
{
    shared_lock<shared_mutex> lk(m_mutex);
    return m_map.at(fd).ip;
}

string UserStatusEvaluator::get_ip(shared_ptr<tcp::socket> socket_ptr) {
    shared_lock<shared_mutex> lk(m_mutex);
    return m_map_boost.at(socket_ptr).ip;
}

string UserStatusEvaluator::get_userId(int fd)
{
    shared_lock<shared_mutex> lk(m_mutex);
    return m_map.at(fd).userId;
}

string UserStatusEvaluator::get_userId(shared_ptr<tcp::socket> socket_ptr) {
    shared_lock<shared_mutex> lk(m_mutex);
    return m_map_boost.at(socket_ptr).userId;
}


