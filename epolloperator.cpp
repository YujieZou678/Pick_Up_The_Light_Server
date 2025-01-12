/*
func: epoll操作者的实现。
author: zouyujie
date: 2025.1.10
*/
#include "epolloperator.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"

EpollOperator::EpollOperator()
{
    /* 创建 */
    m_epollFd = epoll_create(1);  //无效，大于0就行
    if (m_epollFd == -1) {
        perror("epoll_create error");
        exit(1);
    }
}

EpollOperator::~EpollOperator()
{
    /* 关闭 */
    if (close(m_epollFd) == -1) {
        perror("close m_epollFd error");
        exit(1);
    }
}

void EpollOperator::addFd(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;  //可读事件+边缘模式
    ev.data.fd = fd;              //需要被监听的fd
    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl add error");
    }
}

void EpollOperator::deleteFd(int fd)
{
    if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        perror("epoll_ctl del error");
    }
}

int EpollOperator::listen(epoll_event *evs, int size)
{
    int num = epoll_wait(m_epollFd, evs, size, EPOLL_TIME_OUT);
    return num;
}
















