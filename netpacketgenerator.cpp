#include "netpacketgenerator.h"

NetPacketGenerator::NetPacketGenerator()
{
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

NetPacket NetPacketGenerator::sendVideoId_P(const string &videoId)
{
    NetPacket p;
    json jsonMsg;
    jsonMsg["videoId"] = videoId;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::SendFile;
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

    jsonMsg["infotype"] = InfoType::VideoLike;
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

NetPacket NetPacketGenerator::sendNickNameInfo_P(const json &nickNameInfo)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::NickName;
    jsonMsg["nickNameInfo"] = nickNameInfo;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendUserVideoInfo_P(const string &userId, const json &userVideoList)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::UserVideo;
    jsonMsg["userId"] = userId;
    jsonMsg["userVideoList"] = userVideoList;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendUserVideoLikeInfo_P(const string &userId, const json &userVideoLikeList)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::UserVideoLike;
    jsonMsg["userId"] = userId;
    jsonMsg["userVideoLikeList"] = userVideoLikeList;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendMessageInfo_P(const json &messages)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::Message;
    jsonMsg["messages"] = messages;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}

NetPacket NetPacketGenerator::sendVideoPushInfo_P(const json &videos)
{
    NetPacket p;
    json jsonMsg;

    jsonMsg["infotype"] = InfoType::VideoPush;
    jsonMsg["videos"] = videos;

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::GetInfo;
    p.packetHeader.data_size = strMsg.size();

    return p;
}






