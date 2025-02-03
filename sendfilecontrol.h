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

#include "config.h"

class SendFileControl
{
public:
    static SendFileControl *getInstance();
    size_t getFileSize(const char *fileName);  //通过stat结构体获得文件大小，单位字节
    json toJson_FileInfo(const FileType &filetype, const string &id, const string &suffix);
    bool send_file(int fd, const FileType &filetype, const char *filepath, const char *suffix);
    void send_file(int fd, const string &buf); //重载

private:
    SendFileControl();
};

#endif // SENDFILECONTROL_H
