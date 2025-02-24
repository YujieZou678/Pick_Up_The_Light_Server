/*
func: 服务器2，定义了服务器2的操作。
author: zouyujie
date: 2025.2.22
*/
#ifndef BOOSTSERVER_H
#define BOOSTSERVER_H

#include <boost/asio.hpp>

struct NetPacketHeader;

class BoostServer
{
public:
    BoostServer();
    ~BoostServer();
    void launch();  //启动服务器
    void processConnect();  //循环处理客户端连接
    void processSingleRequest(int fd, NetPacketHeader &pheader); //处理单个请求
    void processClientRequest(int fd);  //循环处理客户端请求

private:
    boost::asio::io_context io_context;
};

#endif // BOOSTSERVER_H
