/*
func: 网络包生成器，负责生成需要的网络包。
author: zouyujie
date: 2025.1.22
*/
#ifndef NETPACKETGENERATOR_H
#define NETPACKETGENERATOR_H

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "config.h"

class NetPacketGenerator
{
public:
    static NetPacketGenerator *getInstance();
    NetPacket register_P(bool success);
    NetPacket login_P(bool success);
    NetPacket sendComments_P(const json &comments);
    NetPacket sendVodList_P(const json &vodList);

private:
    NetPacketGenerator();
};

#endif // NETPACKETGENERATOR_H
