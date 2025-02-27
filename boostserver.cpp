#include "boostserver.h"

#include <iostream>
#include <memory>
using std::shared_ptr;
using std::make_shared;
#include <thread>
#include <vector>

#include "session.h"
#include "userstatusevaluator.h"
#include "config.h"

BoostServer::BoostServer() : m_acceptor(boost::asio::ip::tcp::acceptor(
        m_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), LISTEN_PORT)))

{
}

BoostServer::~BoostServer()
{
    m_acceptor.close();
}

void BoostServer::launch()
{
    /* 启动心跳包检测线程 */
    Singleton<UserStatusEvaluator>::getInstance()->start_boost();

    /* 开始监听 */
    start_accept();

    /* 多线程启动事件循环 */
    std::vector<std::thread> threads;
    for (int i=0; i<std::thread::hardware_concurrency(); i++) {
        threads.emplace_back([this](){
            m_io_context.run();
        });
    }
    for (int i=0; i<std::thread::hardware_concurrency(); i++) {
        threads[i].join();  //一直等待
    }
}

void BoostServer::start_accept()
{
    /* 子线程异步接收新连接 */
    std::cout << std::this_thread::get_id() << ":" << "正在接收新连接..." << std::endl;
    shared_ptr<boost::asio::ip::tcp::socket> socket_ptr = make_shared<boost::asio::ip::tcp::socket>(m_io_context);

    m_acceptor.async_accept(*socket_ptr, [socket_ptr, this](const boost::system::error_code &ec){
        if (!ec) {
            std::cout << "Client connected: " << socket_ptr.get()->remote_endpoint() << std::endl;
            /* 心跳 */
            Singleton<UserStatusEvaluator>::getInstance()->add(socket_ptr);
            /* 创建新会话，异步读取数据 */
            std::make_shared<Session>(socket_ptr)->start();
        } else {
            std::cerr << "Accept failed: " << ec.message() << std::endl;
        }

        start_accept();  //一直递归到程序结束
    });
}
