/*
func: 宏，全局变量。
author: zouyujie
date: 2024.12.2
*/
#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
using std::vector;
#include <time.h>

/* 服务端最大连接数 */
#define MAX_CONNECT_COUNT 100

/* 监听端口 */
#define LISTEN_PORT 2222

/* 监听缓冲队列(来不及处理的连接排队数) */
#define LISTEN_BACKLOG 5

/* epoll一次监听最大返回事件数 */
#define MAX_EPOLL_EVENTS 10

/* epoll超时设置 */
#define EPOLL_TIME_OUT -1

/* 一次接收消息最大内存 */
#define MAX_ONCE_RECV 1024

/* 线程池一次销毁或增加线程数 */
#define CHANGE_THREAD_NUMBER 2

/* 线程池最小线程数 */
#define MIN_THREAD_NUMBER 3

/* 线程池最大线程数 */
#define MAX_THREAD_NUMBER 10

/* 数据库名 */
#define DATABASE_NAME "root"

/* 数据库密码 */
#define DATABASE_PASSWORD ""

template <typename T>  //删除数组
inline void safe_delete(T *&target) {
    if (nullptr != target) {
        delete target;
        target = nullptr;
    }
}

template <typename T>  //删除数组指针
inline void safe_delete_arr(T *&target) {
    if (nullptr != target) {
        delete[] target;
        target = nullptr;
    }
}

/* 评论结构体 */
struct MyCommit
{
    const char* str;
    time_t send_date;     //发送时间
    const char* send_id;  //上传的id
};

/* 视频结构体 */
struct MyVedio
{
    vector<const char*> comments;  //评论
    time_t send_date;     //上传时间
    const char* send_id;  //上传的id
    const char* url;      //视频地址
};

/* 客户端请求枚举 */
enum class Purpose {
    Heart,
    Register,
    NewFile
};

/* 注册消息结构体 */
struct Register_Msg {
    char id[20];
    char pw[20];
};

/* 文件信息 */
struct FileInfo
{
    bool is_end;  //是否是最后一个包
    char id[20];    //发送者
};

/* 网络数据包包头 */
struct NetPacketHeader {
    Purpose purpose;        //目的
    unsigned int file_size; //只是需要数据的长度
};

/* 函数的声明 */
int my_recv(int fd, void* buf, int length, int flags);  //确保接收的数据完整(一定为length字节或者错误)
int my_send(int fd, void* buf, int length, int flags);  //确保发送的数据完整

#endif // CONFIG_H
