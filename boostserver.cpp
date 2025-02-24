#include "boostserver.h"

#include <iostream>
using std::cout;
using std::endl;

#include "epolloperator.h"
#include "config.h"

BoostServer::BoostServer()
{
}

BoostServer::~BoostServer()
{
}

void BoostServer::launch()
{

}

void BoostServer::processConnect()
{
//    boost::asio::io_context io_context;
//    // 创建监听器
//    boost::asio::ip::tcp::acceptor acceptor(
//        io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 2222));
//    acceptor.async_accept([](){
//        //
//    });
}

void BoostServer::processSingleRequest(int fd, NetPacketHeader &pheader)
{
}

void BoostServer::processClientRequest(int fd)
{
}
