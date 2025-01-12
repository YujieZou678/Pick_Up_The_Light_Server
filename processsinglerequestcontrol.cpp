/*
func: 处理单个客户端请求控制类的实现。
author: zouyujie
date: 2025.1.12
*/
#include "processsinglerequestcontrol.h"

#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <string>
using std::string;
#include <fstream>
using std::ofstream;

#include "epolloperator.h"
#include "config.h"

/* 静态变量初始化 */
ProcessSingleRequestControl *ProcessSingleRequestControl::m_instance = nullptr;

ProcessSingleRequestControl::ProcessSingleRequestControl()
{
}

ProcessSingleRequestControl *ProcessSingleRequestControl::getInstance()
{
    if (m_instance == nullptr) {
        m_instance = new ProcessSingleRequestControl();
    }
    return m_instance;
}

void ProcessSingleRequestControl::processSingleRequest(int fd, const shared_ptr<EpollOperator> &epollOperator, NetPacketHeader &pheader)
{
    int ret = 0;
    switch (pheader.purpose) {
    case Purpose::Register: {
        std::cout << std::this_thread::get_id() << ": Register" << std::endl;
        /* 2.读网络包数据 */
        Register_Msg re_msg;
        ret = my_recv(fd, &re_msg, pheader.file_size, 0);
        std::cout << re_msg.id << " " << re_msg.pw << std::endl;
    }
    break;
    case Purpose::Heart: {
        std::cout << "心跳包" << std::endl;
    }
    break;
    case Purpose::NewFile: {
        std::cout << "NewFile" << std::endl;
        /* 2.读网络包数据=信息包+数据包 */
        /* 信息包 */
        FileInfo file_info;
        ret = my_recv(fd, &file_info, sizeof(FileInfo), 0);  //阻塞读取文件信息
        /* 数据包 */
        unsigned int data_len = pheader.file_size;  //数据大小
        char* buf = new char[data_len+1];  //申请内存
        ret = my_recv(fd, buf, data_len, 0);  //阻塞读取文件
        std::cout << ret << std::endl;
        /* 将数据写入文件 */
        string filename = string("/root/my_test/Server/test") + file_info.filetype;
        ofstream ofs(filename, std::ios::out|std::ios::app);  //追加写入
        ofs.write(buf, data_len);
        ofs.close();  //关闭文件
        safe_delete_arr(buf);  //释放内存

        /* 重新加入epoll */
        epollOperator.get()->addFd(fd, EPOLLIN|EPOLLET);
    }
    break;
    default:
        break;
    }
}
