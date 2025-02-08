#include "modifyinfocontrol.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include "dbbroker.h"
#include "livelistmonitor.h"
#include "netpacketgenerator.h"
#include "config.h"

ModifyInfoControl::ModifyInfoControl()
{
}

ModifyInfoControl *ModifyInfoControl::getInstance()
{
    static ModifyInfoControl instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

void ModifyInfoControl::modify_info(int fd, const string &buf)
{
    json jsonMsg = json::parse(buf);
    InfoType infotype = jsonMsg["infotype"];
    switch (infotype) {
    case InfoType::Comment: {
        /* 评论 */
        string publisherId = jsonMsg["publisherId"];
        string videoId = jsonMsg["videoId"];
        string content = jsonMsg["content"];
        /* 存入数据库 */
        string command = "insert into Comment(publisherId, videoId, content, time) values('" +
                         publisherId + "','" + videoId + "','" + content + "',(select now()))";
        DbBroker::getInstance()->query_execute(command);
    }
    break;
    case InfoType::Video: {
        /* 删除我的视频 */
        string videoId = jsonMsg["videoId"];
        string command = "delete from Video where id = " + videoId;
        if (DbBroker::getInstance()->query_execute(command)) cout << "已删除视频" << videoId << endl;
        else cerr << "视频删除失败" << endl;
    }
    break;
    case InfoType::Like: {
        /* 点赞(或取消点赞)视频 */
        string videoId = jsonMsg["videoId"];
        string userId = jsonMsg["userId"];
        bool ifLike = jsonMsg["ifLike"];
        if (ifLike) {
            /* 点赞 */
            string command = "insert into VideoLike values('" + videoId + "','" + userId + "')";
            DbBroker::getInstance()->query_execute(command);
        } else {
            /* 取消点赞 */
            string command = "delete from VideoLike where videoId="+videoId+" and userId="+userId;
            DbBroker::getInstance()->query_execute(command);
        }
    }
    break;
    case InfoType::Follow: {
        /* 关注(或取消关注)某人 */
        string userId = jsonMsg["userId"];
        string followerId = jsonMsg["followerId"];
        bool ifFollow = jsonMsg["ifFollow"];
        if (ifFollow) {
            /* 关注某人 */
            string command = "insert into Attention values('" + userId + "','" + followerId + "')";
            DbBroker::getInstance()->query_execute(command);
        } else {
            /* 取消关注某人 */
            string command = "delete from Attention where userId="+userId+" and followerId="+followerId;
            DbBroker::getInstance()->query_execute(command);
        }
    }
    break;
    case InfoType::LiveList: {
        /* 开启(或结束)直播 */
        string userId = jsonMsg["userId"];
        bool ifStart = jsonMsg["ifStart"];
        if (ifStart) {
            /* 开启直播 */
            string url = "rtmp://127.0.0.1:1935/live/"+userId;
            LiveListMonitor::getInstance()->add(userId, url);

            json liveInfo;
            liveInfo["url"] = url;
            NetPacket p = NetPacketGenerator::getInstance()->sendLiveInfo_P(liveInfo);
            my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
        } else {
            /* 结束直播 */
            LiveListMonitor::getInstance()->remove(userId);
        }
    }
    break;
    default:
        break;
    }
}




