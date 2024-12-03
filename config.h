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

/* 声明全局变量 */
extern vector<int> fds;  //所有连接的fd
extern vector<MyVedio> myVedio_live_list;  //直播视频列表信息
extern vector<MyVedio> myVedio_vod_list;   //点播视频列表信息

#endif // CONFIG_H
