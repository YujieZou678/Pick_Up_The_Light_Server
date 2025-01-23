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

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "threadpool.h"
#include "epolloperator.h"
#include "userstatusevaluator.h"
#include "netpacketgenerator.h"
#include "initcontrol.h"
#include "config.h"

MyServer::MyServer()
{
    m_threadPool = make_shared<ThreadPool>(MIN_THREAD_NUMBER, MAX_THREAD_NUMBER);

    /* 单例初始化 */
    m_epollOperator = EpollOperator::getInstance();
    m_userStatusEvaluator = UserStatusEvaluator::getInstance();
    m_initControl = InitControl::getInstance();
    m_netPacketGenerator = NetPacketGenerator::getInstance();
}

MyServer::~MyServer()
{
}

void MyServer::launch()
{
    /* 启动心跳包检测线程 */
    m_userStatusEvaluator->getInstance()->start();

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
    m_epollOperator->addFd(listen_fd, EPOLLIN);  //添加服务器套接字，可读事件+水平模式

    /* epoll开始监听 */
    struct epoll_event evs[MAX_EPOLL_EVENTS];  //一次能返回的最大事件数
    struct sockaddr_in cliaddr;  //获取客户端地址
    socklen_t cliaddr_len = sizeof(cliaddr);

    while (true) {
        int num = m_epollOperator->listen(evs, MAX_EPOLL_EVENTS);
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
                    string addr = inet_ntoa(cliaddr.sin_addr);
                    string port = std::to_string(ntohs(cliaddr.sin_port));
                    string ip = addr+":"+port;
                    std::cout << "新连接：" << ip << std::endl;
                    /* 心跳 */
                    m_userStatusEvaluator->add(new_fd, ip);
                    /* epoll */
                    m_epollOperator->addFd(new_fd, EPOLLIN|EPOLLET);  //可读事件+边缘模式
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
                        /* 心跳 */
                        m_userStatusEvaluator->remove(fd);
                        /* epoll */
                        m_epollOperator->deleteFd(fd);
                        /* close */
                        close(fd);
                        continue;
                    } else if (ret == -1) continue;  //有概率没触发断开而读取失败(UserStatusEvaluator发挥作用)
                    /* 有数据 */
                    if (pheader.purpose == Purpose::NewFile) {
                        m_epollOperator->deleteFd(fd);
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

void MyServer::processSingleRequest(int fd, NetPacketHeader &pheader)
{
    int ret = 0;
    switch (pheader.purpose) {
    case Purpose::Register: {
        std::cout << std::this_thread::get_id() << ": Register" << std::endl;
        /* 2.读数据包 */
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        ret = my_recv(fd, buf, pheader.data_size, 0);
        if (ret==-1 || ret==0) return;
        /* 注册并返回结果 */
        NetPacket p = m_netPacketGenerator->register_P(m_initControl->do_register(buf));
        my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
    }
    break;
    case Purpose::Heart: {
        std::cout << "心跳包" << std::endl;
        /* 当前连接评估状态置0 */
        m_userStatusEvaluator->set_0(fd);
    }
    break;
    case Purpose::NewFile: {
        std::cout << "NewFile" << std::endl;
        /* 2.读文件数据包=文件信息+文件数据 */
        /* 文件信息 */
        char buf[BUF_SIZE+1];
        ret = my_recv(fd, buf, BUF_SIZE, 0);  //阻塞读取文件信息
        json jsonMsg = json::parse(buf);
        /* 文件数据 */
        unsigned int data_size = pheader.data_size;  //数据大小
        char* file_buf = new char[data_size+1];  //申请堆内存
        ret = my_recv(fd, file_buf, data_size, 0);  //阻塞读取文件
        std::cout << ret << std::endl;
        if (ret==-1 || ret==0) return;
        /* 将数据写入文件 */
        string filepath = string("/root/my_test/Server/test") + string(jsonMsg["filetype"]);
        ofstream ofs(filepath);  //覆盖写入
        ofs.write(file_buf, data_size);
        ofs.close();  //关闭文件
        safe_delete_arr(file_buf);  //释放内存

        /* 重新加入epoll */
        EpollOperator::getInstance()->addFd(fd, EPOLLIN|EPOLLET);
    }
    break;
    default:
        break;
    }
}

void MyServer::processClientRequest(int fd, NetPacketHeader pheader)
{
    int ret = 0;
    /* 先处理第一个网络包 */
    processSingleRequest(fd, pheader);
    /* 循环解包，解决粘包问题 */
    while (1) {
        /* 1.读网络包包头 */
        NetPacketHeader pheader;
        ret = my_recv(fd, &pheader, sizeof(NetPacketHeader), MSG_DONTWAIT);  //非阻塞读取
        if (ret > 0) {
            /* 有数据 */
            if (pheader.purpose == Purpose::NewFile) {
                m_epollOperator->deleteFd(fd);
            }
            processSingleRequest(fd, pheader);
        } else if (ret == -1) {
            /* 读取错误，没数据 */
            perror("recv error");
            break;
        }
    }
}




