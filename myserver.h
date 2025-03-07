/*
func: 服务器，定义了服务器的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYSERVER_H
#define MYSERVER_H

#include <string>
using std::string;

class ThreadPool;
class UserStatusEvaluator;
class EpollOperator;
class NetPacketGenerator;
class LiveListMonitor;
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
    int processSingleConnect(int listen_fd);  //处理单个新连接
    void processConnect(int listen_fd);  //循环处理客户端连接
    void processSingleRequest(int fd, NetPacketHeader &pheader); //处理单个请求
    void processClientRequest(int fd);  //循环处理客户端请求

private:
    ThreadPool *m_threadPool;  //线程池对象
    EpollOperator *m_epollOperator;  //epoll操作对象
    UserStatusEvaluator *m_userStatusEvaluator;  //用户连接状态评估者
    NetPacketGenerator *m_netPacketGenerator;    //网络包生成器
    LiveListMonitor *m_liveListMonitor;  //直播列表信息监控者

    InitControl *m_initControl;
    SendFileControl *m_sendFileControl;
    ReceiveFileControl *m_receiveFileControl;
    SendInfoControl *m_sendInfoControl;
    ModifyInfoControl *m_modifyInfoControl;
};

#endif // MYSERVER_H
