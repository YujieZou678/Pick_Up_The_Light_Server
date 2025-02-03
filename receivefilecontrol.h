/*
func: 接收文件控制类。
author: zouyujie
date: 2025.2.2
*/
#ifndef RECEIVEFILECONTROL_H
#define RECEIVEFILECONTROL_H

#include <string>
using std::string;

class ReceiveFileControl
{
public:
    static ReceiveFileControl *getInstance();
    bool receive_file(const string &fileInfo, char *fileData, size_t size);

private:
    ReceiveFileControl();
};

#endif // RECEIVEFILECONTROL_H
