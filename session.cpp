#include "session.h"

#include <iostream>
#include <thread>

#include "userstatusevaluator.h"
#include "netpacketgenerator.h"
#include "initcontrol.h"
#include "sendfilecontrol.h"
#include "receivefilecontrol.h"
#include "sendinfocontrol.h"
#include "modifyinfocontrol.h"

Session::Session(shared_ptr<boost::asio::ip::tcp::socket> socket) :
    m_socket_ptr(socket),
    m_remote_endpoint(socket.get()->remote_endpoint())
{
}

void Session::start()
{
    processClientRequest();
}

void Session::processSingleRequest(NetPacketHeader &pheader)
{
    switch (pheader.purpose) {
    case Purpose::Register: {
        std::cout << "Register" << std::endl;
        /* 2.读数据包 */
        memset(m_buf, 0, sizeof(m_buf));
        m_socket_ptr.get()->read_some(boost::asio::buffer(m_buf, pheader.data_size));
        /* 注册并返回结果 */
        NetPacket p = Singleton<NetPacketGenerator>::getInstance()->register_P(Singleton<InitControl>::getInstance()->do_register(m_buf));
        m_socket_ptr.get()->write_some(boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
    }
    break;
    case Purpose::Login: {
        std::cout << "Login" << std::endl;
        /* 2.读数据包 */
        memset(m_buf, 0, sizeof(m_buf));
        m_socket_ptr.get()->read_some(boost::asio::buffer(m_buf, pheader.data_size));
        /* 登陆并返回结果 */
        NetPacket p = Singleton<NetPacketGenerator>::getInstance()->login_P(Singleton<InitControl>::getInstance()->do_login(m_socket_ptr, m_buf));
        m_socket_ptr.get()->write_some(boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
    }
    break;
    case Purpose::Heart: {
        std::cout << "心跳包" << std::endl;
        /* 当前连接评估状态置0 */
        Singleton<UserStatusEvaluator>::getInstance()->set_0(m_socket_ptr);
    }
    break;
    case Purpose::SendFile: {
        std::cout << "SendFile" << std::endl;
        /* 2.读文件数据包=文件信息+文件数据 */
        /* 文件信息 */
//        char buf[BUF_SIZE+1];
//        ret = my_recv(fd, buf, BUF_SIZE, 0);  //阻塞读取文件信息
//        if (ret==-1 || ret==0 || ret!=BUF_SIZE) return;
//        /* 文件数据 */
//        char* file_buf = new char[pheader.data_size+1];  //申请堆内存
//        ret = my_recv(fd, file_buf, pheader.data_size, 0);  //阻塞读取文件
//        std::cout << ret << std::endl;
//        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
//        /* 接收文件 */
//        m_receiveFileControl->receive_file(buf, file_buf, pheader.data_size);
    }
    break;
    case Purpose::GetFile: {
        std::cout << "GetFile" << std::endl;
        /* 2.读数据包 */
//        char buf[BUF_SIZE];
//        memset(buf, 0, sizeof(buf));
//        ret = my_recv(fd, buf, pheader.data_size, 0);
//        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
//        /* 向客户端发送指定文件 */
//        m_sendFileControl->send_file(fd, buf);
    }
    break;
    case Purpose::GetInfo: {
        std::cout << "GetInfo" << std::endl;
        /* 2.读数据包 */
//        char buf[BUF_SIZE];
//        memset(buf, 0, sizeof(buf));
//        ret = my_recv(fd, buf, pheader.data_size, 0);
//        if (ret==-1 || ret==0 || ret!=pheader.data_size) return;
//        /* 向客户端发送指定信息 */
//        m_sendInfoControl->send_info(fd, buf);
    }
    break;
    case Purpose::ModifyInfo: {
        std::cout << "ModifyInfo" << std::endl;
        /* 2.读数据包 */
        memset(m_buf, 0, sizeof(m_buf));
        m_socket_ptr.get()->read_some(boost::asio::buffer(m_buf, pheader.data_size));
        std::cout << m_buf << std::endl;
//        /* 修改信息 */
//        m_modifyInfoControl->modify_info(fd, buf);
    }
    break;
    default:
        break;
    }
}

void Session::processClientRequest()
{
    auto self = shared_from_this();  //得到当前对象共享指针(异步拷贝传递=数据持久化)
    /* 异步读取包头后，同步读取完整的网络包 */
    boost::asio::async_read(*m_socket_ptr, boost::asio::buffer(&m_pheader, sizeof(m_pheader)), [this, self](const boost::system::error_code &ec, std::size_t length){
        if (!ec) {
            std::cout << std::this_thread::get_id() << ":" << "正在读取网络包..." << std::endl;
            processSingleRequest(m_pheader);  //同步处理

            processClientRequest();  //一直递归，直到断开连接
        } else {
            if (ec == boost::asio::error::connection_reset) {
                /* 断开连接 */
                std::cout << std::this_thread::get_id() << ":" << "正在断开连接..." << std::endl;
                std::cout << "断开连接：" << m_remote_endpoint << std::endl;
                /* 心跳 */
                Singleton<UserStatusEvaluator>::getInstance()->remove(m_socket_ptr);
                /* close */
                m_socket_ptr.get()->close();
                return;
            }
            if (ec == boost::asio::error::operation_aborted) {
                /* UserStatusEvaluator主动断开连接 */
                std::cout << std::this_thread::get_id() << ":" << "正在断开连接..." << std::endl;
                std::cout << "断开连接：" << m_remote_endpoint << std::endl;
                return;
            }
            std::cerr << "async_read failed: " << ec.message() << std::endl;
        }
    });
}

