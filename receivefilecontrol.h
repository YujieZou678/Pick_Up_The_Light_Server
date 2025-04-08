/*
func: 接收文件控制类。
author: zouyujie
date: 2025.2.2
*/
#ifndef RECEIVEFILECONTROL_H
#define RECEIVEFILECONTROL_H

#include <string>
using std::string;
#include <memory>
using std::shared_ptr;
#include <boost/asio.hpp>

#include "noncopyable.h"
#include "singleton.h"

class ReceiveFileControl : private Noncopyable
{
    friend class Singleton<ReceiveFileControl>;  //赋予单例类调用构造权限
public:
    bool receive_file(int fd, const string &fileInfo, char *fileData, size_t size);
    bool receive_file(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const string &fileInfo, char *fileData, size_t size);

private:
    ReceiveFileControl();
};

#endif // RECEIVEFILECONTROL_H
