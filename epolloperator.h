/*
func: epoll操作者。
author: zouyujie
date: 2025.1.10
*/
#ifndef EPOLLOPERATOR_H
#define EPOLLOPERATOR_H

#include <sys/epoll.h>

#include "noncopyable.h"
#include "singleton.h"

class EpollOperator : private Noncopyable
{
    friend class Singleton<EpollOperator>;  //赋予单例类调用构造权限
public:
    void addFd(int fd, uint32_t events);     //增加fd
    void deleteFd(int fd);  //删除fd
    int listen(struct epoll_event *evs, int size);  //开始监听

private:
    EpollOperator();

    int m_epollFd;
};

#endif // EPOLLOPERATOR_H
