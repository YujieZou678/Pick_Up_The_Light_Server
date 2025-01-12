/*
func: 服务器的实现。
author: zouyujie
date: 2024.12.2
*/
#include "myserver.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <fstream>
using std::ofstream;

#include "threadpool.h"
#include "epolloperator.h"
#include "processsinglerequestcontrol.h"
#include "config.h"

MyServer::MyServer()
{
    m_threadPool = make_shared<ThreadPool>(MIN_THREAD_NUMBER, MAX_THREAD_NUMBER);
    m_epollOperator = make_shared<EpollOperator>();
    m_processSingleRequestControl = ProcessSingleRequestControl::getInstance();
}

MyServer::~MyServer()
{
}

void MyServer::launch()
{
    /* 启动心跳包检测线程 */
    //

    /* 启动服务器 */
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
    m_epollOperator.get()->addFd(listen_fd, EPOLLIN);  //添加服务器套接字，可读事件+水平模式

    /* epoll开始监听 */
    struct epoll_event evs[MAX_EPOLL_EVENTS];  //一次能返回的最大事件数
    struct sockaddr_in cliaddr;  //获取客户端地址
    socklen_t cliaddr_len = sizeof(cliaddr);

    while (true) {
        int num = m_epollOperator.get()->listen(evs, MAX_EPOLL_EVENTS);
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
                    /* 心跳 */
//                    string addr = inet_ntoa(cliaddr.sin_addr) + ntohs(cliaddr.sin_port);
//                    unique_lock<mutex> lk(server_mutex);  //加锁
//                    heart_check_map.insert(make_pair(new_fd, make_pair(addr, 0)));
//                    lk.unlock();  //解锁
                    /* epoll */
                    m_epollOperator.get()->addFd(new_fd, EPOLLIN|EPOLLET);  //可读事件+边缘模式
                }
                else {
                    /* 可读事件(有数据或断开)，这里要读取包头 */
                    int ret;
                    NetPacketHeader pheader;
                    ret = my_recv(fd, &pheader, sizeof(NetPacketHeader), MSG_DONTWAIT);  //非阻塞模式读取
                    if (ret == 0) {
                        /* 客户端断开连接，需要删除fd */
                        std::cout << "断开连接：" << inet_ntoa(cliaddr.sin_addr)
                                  << ":" << ntohs(cliaddr.sin_port) << std::endl;
                        m_epollOperator.get()->deleteFd(fd);
                        close(fd);
                        continue;
                    }
                    /* 有数据 */
                    if (pheader.purpose == Purpose::NewFile) {
                        m_epollOperator.get()->deleteFd(fd);
                    }
                    /* 传入子线程 */
                    Task new_fd_task = std::bind(&MyServer::processClientRequest, this, fd, pheader);
                    m_threadPool.get()->add_task(new_fd_task);
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

void MyServer::processClientRequest(int fd, NetPacketHeader pheader)
{
    int ret = 0;
    /* 先处理第一个包 */
    m_processSingleRequestControl->processSingleRequest(fd, m_epollOperator, pheader);
    /* 循环解包，解决粘包问题 */
    while (1) {
        /* 1.读网络包包头 */
        NetPacketHeader pheader;
        ret = my_recv(fd, &pheader, sizeof(NetPacketHeader), MSG_DONTWAIT);  //非阻塞读取
        if (ret > 0) {
            /* 有数据 */
            if (pheader.purpose == Purpose::NewFile) {
                m_epollOperator.get()->deleteFd(fd);
            }
            m_processSingleRequestControl->processSingleRequest(fd, m_epollOperator, pheader);
        } else if (ret == -1) {
            /* 读取错误，没数据 */
            perror("recv error");
            break;
        }
    }
}

//void MyServer::handle_heart_event(int epoll_fd)
//{
//    std::cout << "the heartbeat check thread start..." << std::endl;
//    /* 每3秒轮询检测一次心跳包情况 */
//    while (1) {
//        map<int,pair<string,int>>::iterator it = heart_check_map.begin();
//        for (it; it!=heart_check_map.end(); it++) {
//            pair<int,pair<string,int>> data = *it;
//            if (data.second.second == 3) {  //连续3次检测没有心跳包，则判定客户端断开
//                std::cout << "The client " << data.second.first << " has been offline..." << std::endl;
//                int fd = data.first;
//                close(fd);
//                /* 心跳 */
//                unique_lock<mutex> lk(server_mutex);  //加锁
//                heart_check_map.erase(it++);
//                lk.unlock();
//                /* epoll */
//                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
//                    perror("epoll_ctl error");
//                }
//            } else if (data.second.second < 3 && data.second.second >= 0) {
//                data.second.second += 1;
//                ++it;
//            }
//        }

//        std::this_thread::sleep_for(std::chrono::seconds(3));
//    }
//}



















