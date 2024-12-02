/*
func: 宏，全局变量。
author: zouyujie
date: 2024.12.2
*/
#ifndef CONFIG_H
#define CONFIG_H

/* 服务端最大连接数 */
#define MAX_CONNECT_COUNT 100

/* 评论结构体 */


/* 视频结构体 */
struct MyVedio
{
    vector<const char*> comments;  //评论
    time_t send_date;  //上传时间
    int send_id;       //上传的id
    const char* url;   //视频地址
};

/* 全局变量 */
vector<int> fds;  //所有连接的fd
vector<MyVedio> myVedio_live_list;  //直播视频列表信息
vector<MyVedio> myVedio_vod_list;   //点播视频列表信息

#endif // CONFIG_H
