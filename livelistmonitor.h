/*
func: 直播列表信息监控者，负责管理直播列表信息。
author: zouyujie
date: 2025.1.21
*/
#ifndef LIVELISTMONITOR_H
#define LIVELISTMONITOR_H

#include <mutex>
using std::mutex;
using std::unique_lock;
#include <unordered_map>
using std::unordered_map;
#include <string>
using std::string;

#include "noncopyable.h"
#include "singleton.h"

class LiveListMonitor : private Noncopyable
{
    friend class Singleton<LiveListMonitor>;  //赋予单例类调用构造权限
public:
    void add(const string &id, const string &url);  //增加数据
    void remove(const string &id);  //移除数据
    unordered_map<string,string> getLiveList();  //获取直播列表

private:
    LiveListMonitor();
    mutex m_mutex;
    unordered_map<string,string> m_liveList;
};

#endif // LIVELISTMONITOR_H
