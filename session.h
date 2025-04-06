/*
func: 会话类，实现两端的数据交互。
author: zouyujie
date: 2025.2.25
*/
#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <memory>
using std::shared_ptr;
using std::make_shared;
#include <array>
using std::array;

#include "config.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr);
    void start();

private:
    shared_ptr<boost::asio::ip::tcp::socket> m_socket_ptr;  //套接字
    boost::asio::ip::tcp::endpoint m_remote_endpoint;  //客户端ip

    NetPacketHeader m_pheader;   //包头
    array<char,BUF_SIZE> m_buf;  //数据包

private:
    void processSingleRequest(NetPacketHeader &pheader); //处理单个请求
    void processClientRequest();  //循环处理客户端请求
};

#endif // SESSION_H
