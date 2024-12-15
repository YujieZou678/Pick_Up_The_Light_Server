/*
func: 服务器类，定义了服务器的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYSERVER_H
#define MYSERVER_H

#include <memory>
using std::unique_ptr;
using std::make_unique;

class ThreadPool;  //前置声明
class MyDataBase;

class MyServer
{
public:
    MyServer(const char*, const char*);
    ~MyServer();
    void launch();  //启动服务器

    /* 作为任务加入线程池 */
    static void add_fd(int, int);  //加入一个fd,该fd有可读消息
    static void receive_file(int); //接收文件
    void do_register(int fd,const char*,const char*);  //注册

private:
    const char* ip;
    const char* port;

    unique_ptr<ThreadPool> threadPool;  //线程池对象
    unique_ptr<MyDataBase> myDatabase;  //数据库对象
};

#endif // MYSERVER_H
