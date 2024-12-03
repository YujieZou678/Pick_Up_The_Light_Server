/*
func: 子线程类，封装了一个线程池对象来实现。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYSUBTHREAD_H
#define MYSUBTHREAD_H

class MyDataBase;  //前置声明
class ThreadPool;

class MySubThread
{
public:
    MySubThread();
    ~MySubThread();
    void add_fd(int);  //加入一个fd,该fd有可读消息
    void receive_file(int);  //接收文件
    void update_all_info_live_list();  //实时更新所有客户端直播列表
    void update_all_info_vod_list();   //实时更新所有客户端点播列表

private:
    MyDataBase* myDatabase;  //声明数据库指针
    ThreadPool* threadPool;  //声明线程池指针
};

#endif // MYSUBTHREAD_H
