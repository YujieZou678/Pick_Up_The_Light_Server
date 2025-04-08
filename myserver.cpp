/*
func: 服务器的实现。
author: zouyujie
date: 2024.12.2
*/
#include "myserver.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "threadpool.h"
#include "epolloperator.h"
#include "userstatusevaluator.h"
#include "netpacketgenerator.h"
#include "livelistmonitor.h"
#include "initcontrol.h"
#include "sendfilecontrol.h"
#include "receivefilecontrol.h"
#include "sendinfocontrol.h"
#include "modifyinfocontrol.h"
#include "config.h"

MyServer::MyServer()
{
    /* 单例初始化 */
    m_threadPool = Singleton<ThreadPool>::getInstance();
    m_epollOperator = Singleton<EpollOperator>::getInstance();
    m_userStatusEvaluator = Singleton<UserStatusEvaluator>::getInstance();
    m_netPacketGenerator = Singleton<NetPacketGenerator>::getInstance();
    m_liveListMonitor = Singleton<LiveListMonitor>::getInstance();

    m_initControl = Singleton<InitControl>::getInstance();
    m_sendFileControl = Singleton<SendFileControl>::getInstance();
    m_receiveFileControl = Singleton<ReceiveFileControl>::getInstance();
    m_sendInfoControl = Singleton<SendInfoControl>::getInstance();
    m_modifyInfoControl = Singleton<ModifyInfoControl>::getInstance();
}

MyServer::~MyServer()
{
}

void MyServer::launch()
{
    /* 启动心跳包检测线程 */
    m_userStatusEvaluator->start();

    /* 启动服务器 */
    int listen_fd;
    struct sockaddr_in servaddr;
    int ret = 0;  //返回值

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);  //ipv4 tcp 指定协议特定类型(一般为0)
    if (listen_fd == -1) {
        perror("socket error");
        return;
    }
    int flags = fcntl(listen_fd, F_GETFL, 0);
    fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK);  //socket非阻塞模式

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
    m_epollOperator->addFd(listen_fd, EPOLLIN);  //添加服务器套接字，可读事件+水平模式

    /* epoll开始监听 */
    struct epoll_event evs[MAX_EPOLL_EVENTS];  //一次能返回的最大事件数
    while (true) {
        int num = m_epollOperator->listen(evs, MAX_EPOLL_EVENTS);
        if (num > 0) {
            for (int i=0; i<num; i++) {
                int fd = evs[i].data.fd;  //有事件的fd
                if (fd == listen_fd) {
                    /* 循环非阻塞accept新连接 */
                    processConnect(listen_fd);
                }
                else {
                    /* 可读事件(有数据或断开) */
                    m_epollOperator->deleteFd(fd);  //取掉监听
                    /* 传入子线程 */
                    Task task = std::bind(&MyServer::processClientRequest, this, fd);
                    m_threadPool->add_task(task);
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

int MyServer::processSingleConnect(int listen_fd)
{
    struct sockaddr_in cliaddr;  //获取客户端地址
    socklen_t cliaddr_len = sizeof(cliaddr);

    int new_fd = accept(listen_fd, (struct sockaddr*) &cliaddr, &cliaddr_len);  //非阻塞接收连接
    if (new_fd == -1) {
//        perror("accept error");
        return new_fd;
    }

    string addr = inet_ntoa(cliaddr.sin_addr);
    string port = std::to_string(ntohs(cliaddr.sin_port));
    string ip = addr+":"+port;
    std::cout << "新连接：" << ip << std::endl;
    /* 心跳 */
    m_userStatusEvaluator->add(new_fd, ip);
    /* epoll */
    m_epollOperator->addFd(new_fd, EPOLLIN|EPOLLET);  //可读事件+边缘模式

    return new_fd;
}

void MyServer::processConnect(int listen_fd)
{
    while (processSingleConnect(listen_fd) > 0) {
    }
}

void MyServer::processSingleRequest(int fd, NetPacketHeader &pheader)
{
    int ret = 0;
    switch (pheader.purpose) {
    case Purpose::Register: {
        std::cout << "Register" << std::endl;
        /* 2.读数据包 */
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        ret = my_recv(fd, buf, pheader.data_size, 0);
        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
        /* 注册并返回结果 */
        NetPacket p = m_netPacketGenerator->register_P(m_initControl->do_register(buf));
        my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
    }
    break;
    case Purpose::Login: {
        std::cout << "Login" << std::endl;
        /* 2.读数据包 */
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        ret = my_recv(fd, buf, pheader.data_size, 0);
        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
        /* 登陆并返回结果 */
        NetPacket p = m_netPacketGenerator->login_P(m_initControl->do_login(fd, buf));
        my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
    }
    break;
    case Purpose::Heart: {
        std::cout << "心跳包" << std::endl;
        /* 当前连接评估状态置0 */
        m_userStatusEvaluator->set_0(fd);
    }
    break;
    case Purpose::SendFile: {
        std::cout << "SendFile" << std::endl;
        /* 2.读文件数据包=文件信息+文件数据 */
        /* 文件信息 */
        char buf[BUF_SIZE+1];
        ret = my_recv(fd, buf, BUF_SIZE, 0);  //阻塞读取文件信息
        if (ret==-1 || ret==0 || ret!=BUF_SIZE) return;
        /* 文件数据 */
        char* file_buf = new char[pheader.data_size+1];  //申请堆内存
        ret = my_recv(fd, file_buf, pheader.data_size, 0);  //阻塞读取文件
        std::cout << ret << std::endl;
        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
        /* 接收文件 */
        m_receiveFileControl->receive_file(fd, buf, file_buf, pheader.data_size);
    }
    break;
    case Purpose::GetFile: {
        std::cout << "GetFile" << std::endl;
        /* 2.读数据包 */
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        ret = my_recv(fd, buf, pheader.data_size, 0);
        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
        /* 向客户端发送指定文件 */
        m_sendFileControl->send_file(fd, buf);
    }
    break;
    case Purpose::GetInfo: {
        std::cout << "GetInfo" << std::endl;
        /* 2.读数据包 */
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        ret = my_recv(fd, buf, pheader.data_size, 0);
        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
        /* 向客户端发送指定信息 */
        m_sendInfoControl->send_info(fd, buf);
    }
    break;
    case Purpose::ModifyInfo: {
        std::cout << "ModifyInfo" << std::endl;
        /* 2.读数据包 */
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        ret = my_recv(fd, buf, pheader.data_size, 0);
        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
        /* 修改信息 */
        m_modifyInfoControl->modify_info(fd, buf);
    }
    break;
    default:
        break;
    }
}

void MyServer::processClientRequest(int fd)
{
    int ret;
    /* 循环解包，解决粘包问题 */
    while (1) {
        /* 1.读网络包包头 */
        NetPacketHeader pheader;
        ret = my_recv(fd, &pheader, sizeof(NetPacketHeader), MSG_DONTWAIT);  //非阻塞读取
        if (ret > 0) {
            /* 有数据 */
            if (ret != sizeof(NetPacketHeader)) return;
            processSingleRequest(fd, pheader);
        } else if (ret == -1) {
            /* 读取错误，没数据 */
            if (errno != EAGAIN)
                perror("recv error");
            /* 重新加入监听 */
            Singleton<EpollOperator>::getInstance()->addFd(fd, EPOLLIN|EPOLLET);
            break;
        } else if (ret == 0) {
            /* 断开连接 */
            std::cout << "断开连接：" << m_userStatusEvaluator->get_ip(fd) << std::endl;
            /* 直播列表信息 */
            m_liveListMonitor->remove(m_userStatusEvaluator->get_userId(fd));
            /* 心跳 */
            m_userStatusEvaluator->remove(fd);
            /* epoll */
            m_epollOperator->deleteFd(fd);
            /* close */
            close(fd);
            break;
        }
    }
}




