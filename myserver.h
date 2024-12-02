/*
func: 服务器类，定义了服务器的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYSERVER_H
#define MYSERVER_H


class MyServer
{
public:
    MyServer(const char*, const char*);
    ~MyServer();
    void launch();  //启动服务器

private:
    const char* ip;
    const char* port;
};

#endif // MYSERVER_H
