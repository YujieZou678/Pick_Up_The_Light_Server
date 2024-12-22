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
#include <fstream>
using std::ofstream;

#include "config.h"
#include "threadpool.h"
#include "mydatabase.h"

/* 静态变量初始化 */
ThreadPool MyServer::threadPool = ThreadPool(MIN_THREAD_NUMBER, MAX_THREAD_NUMBER);
MyDataBase MyServer::myDatabase = MyDataBase(DATABASE_NAME, DATABASE_PASSWORD);

MyServer::MyServer(const char* ip, const char* port)
{
    this->ip = ip;
    this->port = port;
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

    /* 启动心跳包检测线程 */
    Task heart_check_task = std::bind(&MyServer::handle_heart_event, this, epoll_fd);
    MyServer::threadPool.add_task(heart_check_task);

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
                    /* 心跳 */
                    string addr = inet_ntoa(cliaddr.sin_addr) + ntohs(cliaddr.sin_port);
                    unique_lock<mutex> lk(server_mutex);  //加锁
                    heart_check_map.insert(make_pair(new_fd, make_pair(addr, 0)));
                    lk.unlock();  //解锁
                    /* epoll */
                    ev.data.fd = new_fd;
                    ev.events = EPOLLIN|EPOLLET;  //可读事件+边缘模式
                    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &ev);
                    if (ret == -1) {
                        perror("epoll_ctl error");
                        continue;
                    }
                }
                else {
                    /* 可读事件(有数据或断开)，这里要读取包头 */
                    int ret;
                    NetPacketHeader pheader;
                    ret = my_recv(fd, &pheader, sizeof(NetPacketHeader), 0);
                    if (ret == 0) {
                        /* 客户端断开连接，需要删除fd */
                        std::cout << "断开连接：" << inet_ntoa(cliaddr.sin_addr)
                                  << ":" << ntohs(cliaddr.sin_port) << std::endl;
                        ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        if (ret == -1) {
                            perror("epoll_ctl error");
                        }
                        close(fd);
                        continue;
                    }
                    /* 有数据 */
                    if (pheader.purpose == Purpose::NewFile) {
                        ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        if (ret == -1) {
                            perror("epoll_ctl error");
                        }
                    }
                    /* 传入子线程 */
                    Task new_fd_task = std::bind(&MyServer::handle_fd_event, this, fd, epoll_fd, pheader);
                    MyServer::threadPool.add_task(new_fd_task);
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

void MyServer::handle_heart_event(int epoll_fd)
{
    std::cout << "the heartbeat check thread start..." << std::endl;
    /* 每3秒轮询检测一次心跳包情况 */
    while (1) {
        map<int,pair<string,int>>::iterator it = heart_check_map.begin();
        for (it; it!=heart_check_map.end(); it++) {
            pair<int,pair<string,int>> data = *it;
            if (data.second.second == 3) {  //连续3次检测没有心跳包，则判定客户端断开
                std::cout << "The client " << data.second.first << " has been offline..." << std::endl;
                int fd = data.first;
                close(fd);
                /* 心跳 */
                unique_lock<mutex> lk(server_mutex);  //加锁
                heart_check_map.erase(it++);
                lk.unlock();
                /* epoll */
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
                    perror("epoll_ctl error");
                }
            } else if (data.second.second < 3 && data.second.second >= 0) {
                data.second.second += 1;
                ++it;
            }
        }

        sleep(3);
    }
}

void MyServer::switch_purpose(int fd, int epoll_fd, NetPacketHeader &pheader)
{
    int ret = 0;
    switch (pheader.purpose) {
    case Purpose::Register: {
        std::cout << std::this_thread::get_id() << ": Register" << std::endl;
        /* 2.读网络包数据 */
        Register_Msg re_msg;
        ret = my_recv(fd, &re_msg, pheader.file_size, 0);
        std::cout << re_msg.id << " " << re_msg.pw << std::endl;
    }
    break;
    case Purpose::Heart: {
//        std::cout << "心跳包" << std::endl;
        unique_lock<mutex> lk(server_mutex);  //加锁
        heart_check_map[fd].second = 0;
        lk.unlock();
    }
    break;
    case Purpose::NewFile: {
        std::cout << "NewFile" << std::endl;
        /* 2.读网络包数据=信息包+数据包 */
        /* 信息包 */
        FileInfo file_info;
        ret = my_recv(fd, &file_info, sizeof(FileInfo), 0);
        /* 数据包 */
        unsigned int data_len = pheader.file_size;  //数据大小
        char* buf = new char[data_len+1];  //申请内存
        ret = my_recv(fd, buf, data_len, 0);
        std::cout << ret << std::endl;
        /* 将数据写入文件 */
        ofstream ofs("/root/my_test/Server/test.png", std::ios::out|std::ios::app);  //追加写入
        ofs.write(buf, data_len);
        ofs.close();  //关闭文件
        safe_delete_arr(buf);  //释放内存
        /* 判断是否是最后一个包 */
        if (file_info.is_end) {
            /* 大文件的最后一个包 */
            std::cout << "end" << std::endl;
            /* 给客户端确认传输完毕 */
            //
        }

        /* 重新加入epoll */
        struct epoll_event ev;
        ev.events = EPOLLIN|EPOLLET;  //可读事件，边缘模式
        ev.data.fd = fd;  //需要监听的fd
        ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
        if (ret == -1) {
            perror("epoll_ctl error");
        }
    }
    break;
    default:
        break;
    }
}

/* 参数：fd epoll_fd */
void MyServer::handle_fd_event(int fd, int epoll_fd, NetPacketHeader pheader)
{
    int ret = 0;
    /* 先处理第一个包 */
    switch_purpose(fd, epoll_fd, pheader);
    /* 循环解包，解决粘包问题 */
    while (1) {
        /* 1.读网络包包头 */
        NetPacketHeader pheader;
        ret = my_recv(fd, &pheader, sizeof(NetPacketHeader), MSG_DONTWAIT);  //非阻塞读取
        if (ret > 0) {
            /* 有数据 */
            if (pheader.purpose == Purpose::NewFile) {
                ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                if (ret == -1) {
                    perror("epoll_ctl error");
                }
            }
            switch_purpose(fd, epoll_fd, pheader);
        } else if (ret == -1) {
            /* 读取错误，没数据 */
            perror("recv error");
            break;
        }
    }
}

/* 参数：fd id password */
void MyServer::do_register(int fd, const char *id, const char *password)
{
    if (MyServer::myDatabase.check_ID(id)) {
        std::cout << "账号存在" << std::endl;
    } else std::cout << "账号不存在" << std::endl;
}



















