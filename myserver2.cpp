#include "myserver2.h"

#include <boost/asio.hpp>
#include <iostream>
using std::cout;
using std::endl;

#include "myserver.h"
#include "epolloperator.h"
#include "config.h"

MyServer2::MyServer2()
{
    m_myServer = new MyServer;
}

MyServer2::~MyServer2()
{
    safe_delete(m_myServer);
}

void MyServer2::launch()
{

}

void MyServer2::processConnect()
{
//    boost::asio::io_context io_context;
//    // 创建监听器
//    boost::asio::ip::tcp::acceptor acceptor(
//        io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 2222));
//    acceptor.async_accept([](){
//        //
//    });
}

void MyServer2::processSingleRequest(int fd, NetPacketHeader &pheader)
{
    m_myServer->processSingleRequest(fd, pheader);
}

void MyServer2::processClientRequest(int fd)
{
    m_myServer->processClientRequest(fd);
}
