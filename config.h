/*
func: 宏，全局变量。
author: zouyujie
date: 2024.12.2
*/
#ifndef CONFIG_H
#define CONFIG_H

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

/* 普通数据包 */
#define BUF_SIZE 1024

/* 文件数据包 1G */
#define FILE_BUF_SIZE 1024*1024*1024

/* 线程池一次销毁或增加线程数 */
#define CHANGE_THREAD_NUMBER 2

/* 线程池最小线程数 */
#define MIN_THREAD_NUMBER 3

/* 线程池最大线程数 */
#define MAX_THREAD_NUMBER 10

/* 用户头像存储地址 */
#define PROFILE_PICTURE_URL "/root/my_test/Server/cache/profilePicture/"

/* 点播视频存储地址 */
#define VIDEO_URL "/opt/nginx-video/"

template <typename T>  //删除指针
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

/* 客户端请求枚举 */
enum class Purpose {
    Heart,
    Register,
    Login,
    SendFile,
    GetFile,
    GetInfo,
    ModifyInfo
};

/* 文件类型 */
enum class FileType {
    ProfilePicture,
    Video
};

/* 信息类型 */
enum class InfoType {
    Comment,
    Like,
    Follow,
    Fans,
    VodList,
    LiveList,
    Video
};

/* 网络包包头 */
struct NetPacketHeader {
    Purpose purpose;         //目的
    unsigned int data_size;  //数据长度
};

/* 数据包 */
struct DataPacket {
    char data[BUF_SIZE];
};
struct FileDataPacket {
    char fileInfo[BUF_SIZE];       //文件信息
    char fileData[FILE_BUF_SIZE];  //文件数据
};

/* 网络包 */
struct NetPacket {
    NetPacketHeader packetHeader;  //包头
    DataPacket dataPacket;         //数据包
};
struct FileNetPacket {
    NetPacketHeader packetHeader;  //包头
    FileDataPacket fileDataPacket; //文件数据包
};

/* 函数的声明 */
int my_recv(int fd, void* buf, int length, int flags);  //确保接收的数据完整(一定为length字节或者错误)
int my_send(int fd, void* buf, int length, int flags);  //确保发送的数据完整

#endif // CONFIG_H
