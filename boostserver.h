/*
func: 服务器2，定义了服务器2的操作。
author: zouyujie
date: 2025.2.22
*/
#ifndef BOOSTSERVER_H
#define BOOSTSERVER_H

#include <boost/asio.hpp>

class BoostServer
{
public:
    BoostServer();
    ~BoostServer();
    void launch();  //启动服务器
    void start_accept();  //开始接收连接

private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::acceptor m_acceptor;
};

#endif // BOOSTSERVER_H
