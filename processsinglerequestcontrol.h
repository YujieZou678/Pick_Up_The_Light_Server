/*
func: 处理单个客户端请求控制类。
author: zouyujie
date: 2025.1.12
*/
#ifndef PROCESSSINGLEREQUESTCONTROL_H
#define PROCESSSINGLEREQUESTCONTROL_H

#include <memory>
using std::shared_ptr;

class EpollOperator;
struct NetPacketHeader;

class ProcessSingleRequestControl
{
public:
    static ProcessSingleRequestControl *getInstance();
    void processSingleRequest(int fd, const shared_ptr<EpollOperator> &epollOperator, NetPacketHeader &pheader);

private:
    ProcessSingleRequestControl();
    static ProcessSingleRequestControl *m_instance;
};

#endif // PROCESSSINGLEREQUESTCONTROL_H
