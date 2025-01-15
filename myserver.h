/*
func: 服务器，定义了服务器的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYSERVER_H
#define MYSERVER_H

#include <string>
using std::string;
#include <memory>
using std::shared_ptr;
using std::make_shared;

class ThreadPool;
class UserStatusEvaluator;
class EpollOperator;
class ProcessSingleRequestControl;
struct NetPacketHeader;

class MyServer
{
public:
    MyServer();
    ~MyServer();
    void launch();  //启动服务器
    void processClientRequest(int fd, NetPacketHeader pheader);  //处理客户端请求

private:
    shared_ptr<ThreadPool> m_threadPool;  //线程池对象
    shared_ptr<UserStatusEvaluator> m_userStatusEvaluator;  //用户连接状态评估对象

    EpollOperator *m_epollOperator;  //epoll操作对象
    ProcessSingleRequestControl *m_processSingleRequestControl;
};

#endif // MYSERVER_H
