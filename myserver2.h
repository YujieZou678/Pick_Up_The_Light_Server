/*
func: 服务器2，定义了服务器2的操作。
author: zouyujie
date: 2025.2.22
*/
#ifndef MYSERVER2_H
#define MYSERVER2_H

class MyServer;
struct NetPacketHeader;

class MyServer2
{
public:
    MyServer2();
    ~MyServer2();
    void launch();  //启动服务器
    void processConnect();  //循环处理客户端连接
    void processSingleRequest(int fd, NetPacketHeader &pheader); //处理单个请求
    void processClientRequest(int fd);  //循环处理客户端请求

private:
    MyServer *m_myServer;  //聚合(合成复用原则)
};

#endif // MYSERVER2_H
