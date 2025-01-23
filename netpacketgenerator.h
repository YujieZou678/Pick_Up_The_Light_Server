/*
func: 网络包生成器，负责生成需要的网络包。
author: zouyujie
date: 2025.1.22
*/
#ifndef NETPACKETGENERATOR_H
#define NETPACKETGENERATOR_H

#include "config.h"

class NetPacketGenerator
{
public:
    static NetPacketGenerator *getInstance();
    NetPacket register_P(bool success);

private:
    NetPacketGenerator();
};

#endif // NETPACKETGENERATOR_H
