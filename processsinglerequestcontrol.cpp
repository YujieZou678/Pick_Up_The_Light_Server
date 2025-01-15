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

#include "nlohmann/json.hpp"
using json = nlohmann::json;

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

void ProcessSingleRequestControl::processSingleRequest(int fd, NetPacketHeader &pheader)
{
    int ret = 0;
    switch (pheader.purpose) {
    case Purpose::Register: {
        std::cout << std::this_thread::get_id() << ": Register" << std::endl;
        /* 2.读数据包 */
        char buf[BUF_SIZE];
        ret = my_recv(fd, buf, pheader.data_size, 0);
        json jsonMsg = json::parse(buf);
        std::cout << jsonMsg["id"] << " " << jsonMsg["password"] << std::endl;
    }
    break;
    case Purpose::Heart: {
        std::cout << "心跳包" << std::endl;
    }
    break;
    case Purpose::NewFile: {
        std::cout << "NewFile" << std::endl;
        /* 2.读文件数据包=文件信息+文件数据 */
        /* 文件信息 */
        char buf[BUF_SIZE+1];
        ret = my_recv(fd, buf, BUF_SIZE, 0);  //阻塞读取文件信息
        json jsonMsg = json::parse(buf);
        /* 文件数据 */
        unsigned int data_size = pheader.data_size;  //数据大小
        char* file_buf = new char[data_size+1];  //申请内存
        ret = my_recv(fd, file_buf, data_size, 0);  //阻塞读取文件
        std::cout << ret << std::endl;
        /* 将数据写入文件 */
        string filepath = string("/root/my_test/Server/test") + string(jsonMsg["filetype"]);
        ofstream ofs(filepath);  //覆盖写入
        ofs.write(file_buf, data_size);
        ofs.close();  //关闭文件
        safe_delete_arr(file_buf);  //释放内存

        /* 重新加入epoll */
        EpollOperator::getInstance()->addFd(fd, EPOLLIN|EPOLLET);
    }
    break;
    default:
        break;
    }
}
