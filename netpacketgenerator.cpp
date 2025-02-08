#include "netpacketgenerator.h"

#include <string>
using std::string;

NetPacketGenerator::NetPacketGenerator()
{
}

NetPacketGenerator *NetPacketGenerator::getInstance()
{
    static NetPacketGenerator instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

NetPacket NetPacketGenerator::register_P(bool success)
{
    NetPacket p;
    json jsonMsg;

    if (success) {
        jsonMsg["success"] = "true";
    } else {
        jsonMsg["success"] = "false";
    }

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::Register;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::login_P(bool success)
{
    NetPacket p;
    json jsonMsg;

    if (success) {
        jsonMsg["success"] = "true";
    } else {
        jsonMsg["success"] = "false";
    }

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::Login;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendComments_P(const json &comments)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::Comment;
    jsonMsg["comments"] = comments;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendVodList_P(const json &vodList)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::VodList;
    jsonMsg["vodList"] = vodList;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendLikeInfo_P(const json &likeInfo)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::Like;
    jsonMsg["likeInfo"] = likeInfo;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendFollowInfo_P(const json &followInfo)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::Follow;
    jsonMsg["followInfo"] = followInfo;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendFansInfo_P(const json &fansInfo)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::Fans;
    jsonMsg["fansInfo"] = fansInfo;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendLiveInfo_P(const json &liveInfo)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::LiveList;
    jsonMsg["liveInfo"] = liveInfo;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::ModifyInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendLiveList_P(const json &liveList)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::LiveList;
    jsonMsg["liveList"] = liveList;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}






