/*
func: 接收文件控制类。
author: zouyujie
date: 2025.2.2
*/
#ifndef RECEIVEFILECONTROL_H
#define RECEIVEFILECONTROL_H

#include <string>
using std::string;

#include "noncopyable.h"
#include "singleton.h"

class ReceiveFileControl : public Noncopyable
{
    friend class Singleton<ReceiveFileControl>;  //赋予单例类调用构造权限
public:
    bool receive_file(const string &fileInfo, char *fileData, size_t size);

private:
    ReceiveFileControl();
};

#endif // RECEIVEFILECONTROL_H
