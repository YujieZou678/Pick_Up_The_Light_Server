/*
func: 服务器类，定义了服务器的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYSERVER_H
#define MYSERVER_H

#include <map>
using std::map;
using std::pair;
#include <string>
using std::string;
#include <mutex>
using std::mutex;
using std::unique_lock;
#include <vector>
using std::vector;

class ThreadPool;  //前置声明
class MyDataBase;
class MyVedio;
class NetPacketHeader;

class MyServer
{
public:
    MyServer(const char*, const char*);
    ~MyServer();
    void launch();  //启动服务器

private:
    const char* ip;
    const char* port;
    mutex server_mutex;  //服务器的锁
    map<int,pair<string,int>> heart_check_map; //心跳检测map
    vector<MyVedio> myVedio_live_list;  //直播视频列表信息
    vector<MyVedio> myVedio_vod_list;   //点播视频列表信息

    void handle_heart_event(int);    //处理心跳包事件(独立一个线程)
    void switch_purpose(int, int, NetPacketHeader&);  //目的分类
    void handle_fd_event(int, int, NetPacketHeader);  //处理一个fd的事件

    /* 静态变量 */
    static MyDataBase myDatabase;  //数据库对象
    static ThreadPool threadPool;  //线程池对象

    /* 作为任务加入线程池 */
    static void do_register(int fd,const char*,const char*);  //注册
};

#endif // MYSERVER_H
