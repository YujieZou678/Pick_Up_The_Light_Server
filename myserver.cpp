/*
func: 服务器类的实现。
author: zouyujie
date: 2024.12.2
*/
#include "myserver.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include <iostream>
#include <stdexcept>

#include "threadpool.h"
#include "mydatabase.h"
#include "config.h"

MyServer::MyServer(const char* ip, const char* port)
{
    this->ip = ip;
    this->port = port;

    this->threadPool = make_unique<ThreadPool>(MIN_THREAD_NUMBER, MAX_THREAD_NUMBER);
    try {
        this->myDatabase = make_unique<MyDataBase>(DATABASE_NAME, DATABASE_PASSWORD);
    } catch (std::domain_error e) {
        std::cerr << e.what() << std::endl;
    }
}

MyServer::~MyServer()
{
}

void MyServer::launch()
{
    /* 启动服务器，开始监听处理连接 */
    int listen_fd;
    struct sockaddr_in servaddr;
    int ret = 0;  //返回值

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);  //ipv4 tcp 指定协议特定类型(一般为0)
    if (listen_fd == -1) {
        perror("socket error");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));  //初始化内存为0
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(LISTEN_PORT);

    ret = bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr));  //绑定
    if (ret == -1) {
        perror("bind error");
        return;
    }

    ret = listen(listen_fd, LISTEN_BACKLOG);
    if (ret == -1) {
        perror("listen error");
        return;
    }

    /* epoll IO多路复用 */
    int epoll_fd = epoll_create(1);  //无效，大于0就行
    if (epoll_fd == -1) {
        perror("epoll_create error");
        return;
    }

    /* 添加需要监听的fd */
    struct epoll_event ev;
    ev.events = EPOLLIN;  //可读事件
    ev.data.fd = listen_fd;  //需要监听的fd
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
    if (ret == -1) {
        perror("epoll_ctl error");
        return;
    }

    /* epoll开始监听 */
    struct epoll_event evs[MAX_EPOLL_EVENTS];  //一次能返回的最大事件数
    struct sockaddr_in cliaddr;  //获取客户端地址
    socklen_t cliaddr_len = sizeof(cliaddr);
    while (true) {
        int num = epoll_wait(epoll_fd, evs, MAX_EPOLL_EVENTS, EPOLL_TIME_OUT);
        if (num > 0) {
            for (int i=0; i<num; i++) {
                int fd = evs[i].data.fd;  //有事件的fd
                if (fd == listen_fd) {
                    /* 处理新连接 */
                    int new_fd = accept(listen_fd, (struct sockaddr*) &cliaddr, &cliaddr_len);
                    if (new_fd == -1) {
                        perror("accept error");
                        continue;
                    }
                    std::cout << "新连接：" << inet_ntoa(cliaddr.sin_addr)
                              << ":" << ntohs(cliaddr.sin_port) << std::endl;
                    ev.data.fd = new_fd;
                    ev.events = EPOLLIN|EPOLLET;  //可读事件+边缘模式
                    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &ev);
                    if (ret == -1) {
                        perror("epoll_ctl error");
                        continue;
                    }
                }
                else {
                    /* 处理新消息 */
                    this->threadPool.get()->add_task(add_fd, fd, epoll_fd);
                }
            }
        } else if (num == 0) {
            /* 超时 */
            std::cout << "EPOLL：timeout..." << std::endl;
        } else {
            /* 出错 */
            perror("epoll_wait error");
            return;
        }
    }  // 循环
}

/* 参数：fd epoll_fd */
void MyServer::add_fd(int fd, int epoll_fd)
{
    int ret = 0;  //返回值
    struct sockaddr_in cliaddr;  //获取客户端地址
    socklen_t cliaddr_len = sizeof(cliaddr);

    /* 读取客户端请求 */
    char buf[MAX_ONCE_RECV];
    memset(buf, 0, sizeof(buf));
    ret = recv(fd, buf, sizeof(buf), 0);
    if (ret > 0) {
        /* 接收到消息 */
        std::cout << buf << std::endl;
        //
        //
    } else if (ret == 0) {
        /* 客户端断开连接，需要删除fd */
        ret = getpeername(fd, (struct sockaddr*) &cliaddr, &cliaddr_len);
        if (ret == -1) {
            perror("getpeername error");
        }
        std::cout << "断开连接：" << inet_ntoa(cliaddr.sin_addr)
                  << ":" << ntohs(cliaddr.sin_port) << std::endl;
        ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        if (ret == -1) {
            perror("epoll_ctl error");
        }
        close(fd);
    } else {
        /* 读取错误 */
        perror("recv error");
    }
}

/* 参数：fd id password */
void MyServer::do_register(int fd, const char *id, const char *password)
{
    if (myDatabase.get()->check_ID(id)) {
        std::cout << "账号存在" << std::endl;
    } else std::cout << "账号不存在" << std::endl;
}



















