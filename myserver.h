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
class NetPacketGenerator;
struct NetPacketHeader;
class InitControl;
class SendFileControl;
class ReceiveFileControl;
class SendInfoControl;
class ModifyInfoControl;

class MyServer
{
public:
    MyServer();
    ~MyServer();
    void launch();  //启动服务器
    void processSingleRequest(int fd, NetPacketHeader &pheader); //处理单个请求
    void processClientRequest(int fd);  //循环处理客户端请求

private:
    shared_ptr<ThreadPool> m_threadPool;  //线程池对象
    EpollOperator *m_epollOperator;  //epoll操作对象
    UserStatusEvaluator *m_userStatusEvaluator;  //用户连接状态评估者
    NetPacketGenerator *m_netPacketGenerator;    //网络包生成器

    InitControl *m_initControl;
    SendFileControl *m_sendFileControl;
    ReceiveFileControl *m_receiveFileControl;
    SendInfoControl *m_sendInfoControl;
    ModifyInfoControl *m_modifyInfoControl;
};

#endif // MYSERVER_H
