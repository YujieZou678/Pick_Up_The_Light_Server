#include "livelistmonitor.h"

LiveListMonitor::LiveListMonitor()
{
}

LiveListMonitor *LiveListMonitor::getInstance()
{
    static LiveListMonitor instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

void LiveListMonitor::add(const string &id, const string &url)
{
    unique_lock<mutex> l(m_mutex);
    m_liveList.insert(std::make_pair(id, url));
}

void LiveListMonitor::remove(const string &id)
{
    unique_lock<mutex> l(m_mutex);
    m_liveList.erase(id);
}

unordered_map<string, string> LiveListMonitor::getLiveList()
{
    unique_lock<mutex> l(m_mutex);
    return m_liveList;
}
