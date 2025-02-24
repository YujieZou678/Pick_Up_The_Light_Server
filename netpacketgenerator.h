/*
func: 网络包生成器，负责生成需要的网络包。
author: zouyujie
date: 2025.1.22
*/
#ifndef NETPACKETGENERATOR_H
#define NETPACKETGENERATOR_H

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "noncopyable.h"
#include "singleton.h"
#include "config.h"

class NetPacketGenerator : private Noncopyable
{
    friend class Singleton<NetPacketGenerator>;  //赋予单例类调用构造权限
public:
    NetPacket register_P(bool success);
    NetPacket login_P(bool success);
    NetPacket sendComments_P(const json &comments);
    NetPacket sendVodList_P(const json &vodList);
    NetPacket sendLikeInfo_P(const json &likeInfo);
    NetPacket sendFollowInfo_P(const json &followInfo);
    NetPacket sendFansInfo_P(const json &fansInfo);
    NetPacket sendLiveInfo_P(const json &liveInfo);
    NetPacket sendLiveList_P(const json &liveList);

private:
    NetPacketGenerator();
};

#endif // NETPACKETGENERATOR_H
