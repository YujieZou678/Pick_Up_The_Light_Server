#include "livelistmonitor.h"

LiveListMonitor::LiveListMonitor()
{
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
