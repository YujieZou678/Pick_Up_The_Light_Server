/*
func: 发送文件控制类。
author: zouyujie
date: 2025.1.26
*/
#ifndef SENDFILECONTROL_H
#define SENDFILECONTROL_H

#include <string>
using std::string;
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <boost/asio.hpp>
#include <memory>
using std::shared_ptr;

#include "noncopyable.h"
#include "singleton.h"
#include "config.h"

class SendFileControl : private Noncopyable
{
    friend class Singleton<SendFileControl>;  //赋予单例类调用构造权限
public:
    size_t getFileSize(const char *fileName);  //通过stat结构体获得文件大小，单位字节
    void send_file(int fd, const string &buf);
    void send_file(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const string &buf);  //重载

private:
    SendFileControl();
    json toJson_PFileInfo(const FileType &filetype, const string &id, const string &suffix);
    json toJson_VFileInfo(const FileType &filetype, const string &videoId, const string &publisherId, const string &suffix);
    bool send_pfile(int fd, const FileType &filetype, const char *filepath, const string &id, const string &suffix);
    bool send_pfile(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const FileType &filetype, const char *filepath, const string &id, const string &suffix);  //重载
    bool send_vfile(int fd, const FileType &filetype, const char *filepath, const string &videoId, const string &publisherId, const string &suffix);
    bool send_vfile(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const FileType &filetype, const char *filepath, const string &videoId, const string &publisherId, const string &suffix);  //重载
};

#endif // SENDFILECONTROL_H
