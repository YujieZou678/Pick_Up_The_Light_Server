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
#include "snowflakeidworker.h"
#include "config.h"

ModifyInfoControl::ModifyInfoControl()
{
}

void ModifyInfoControl::modify_info(int fd, const string &buf)
{
    json jsonMsg = json::parse(buf);
    InfoType infotype = jsonMsg["infotype"];
    switch (infotype) {
    case InfoType::Comment: {
        /* 评论 */
        string id = std::to_string(Singleton<IdWorker>::getInstance()->nextId());
        string replyCommentId = jsonMsg["replyCommentId"];
        string publisherId = jsonMsg["publisherId"];
        string videoId = jsonMsg["videoId"];
        string content = jsonMsg["content"];
        /* 存入数据库 */
        string command = "insert into Comment(id, replyCommentId, publisherId, videoId, content, time) values('" +
                         id + "', '" + replyCommentId + "', '" + publisherId + "','" + videoId + "','" + content + "',(select now()))";
        Singleton<DbBroker>::getInstance()->query_execute(command);
    }
    break;
    case InfoType::Video: {
        string videoId = jsonMsg["videoId"];
        /* 删除视频文件 */
        string videoSuffix;
        string command = "select videoSuffix from Video where id=" + videoId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                auto it = res.begin();
                mysqlpp::Row row = *it;
                videoSuffix = string(row[0]);
            }
        }
        else cerr << "query.store() failed!" << endl;

        string dirpath = VIDEO_URL;
        string filepath = dirpath + videoId + videoSuffix;
        if (remove(filepath.data()) == 0) cout << "已删除视频文件" << videoId << endl;
        else cerr << "视频文件删除失败" << endl;
        /* 删除视频相关信息（视频信息+视频点赞信息） */
        //视频信息
        command = "delete from Video where id = " + videoId;
        if (Singleton<DbBroker>::getInstance()->query_execute(command)) cout << "已删除视频信息" << videoId << endl;
        else cerr << "视频删除失败" << endl;
        //视频点赞信息
        command = "delete from VideoLike where videoId = " + videoId;
        if (Singleton<DbBroker>::getInstance()->query_execute(command)) cout << "已删除视频点赞信息" << videoId << endl;
        else cerr << "视频点赞信息删除失败" << endl;
    }
    break;
    case InfoType::VideoLike: {
        /* 点赞(或取消点赞)视频 */
        string videoId = jsonMsg["id"];
        string userId = jsonMsg["userId"];
        bool ifLike = jsonMsg["ifLike"];
        if (ifLike) {
            /* 点赞 */
            string command = "insert into VideoLike values('" + videoId + "','" + userId + "')";
            Singleton<DbBroker>::getInstance()->query_execute(command);
        } else {
            /* 取消点赞 */
            string command = "delete from VideoLike where videoId="+videoId+" and userId="+userId;
            Singleton<DbBroker>::getInstance()->query_execute(command);
        }
    }
    break;
    case InfoType::CommentLike: {
        /* 点赞(或取消点赞)评论 */
        string commentId = jsonMsg["id"];
        string userId = jsonMsg["userId"];
        bool ifLike = jsonMsg["ifLike"];
        if (ifLike) {
            /* 点赞 */
            string command = "insert into CommentLike values('" + commentId + "','" + userId + "')";
            Singleton<DbBroker>::getInstance()->query_execute(command);
        } else {
            /* 取消点赞 */
            string command = "delete from CommentLike where videoId=" + commentId + " and userId=" + userId;
            Singleton<DbBroker>::getInstance()->query_execute(command);
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
            string command = "insert into Follow values('" + userId + "','" + followerId + "')";
            Singleton<DbBroker>::getInstance()->query_execute(command);
        } else {
            /* 取消关注某人 */
            string command = "delete from Follow where userId="+userId+" and followerId="+followerId;
            Singleton<DbBroker>::getInstance()->query_execute(command);
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
            Singleton<LiveListMonitor>::getInstance()->add(userId, url);

            json liveInfo;
            liveInfo["url"] = url;
            NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLiveInfo_P(liveInfo);
            my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
        } else {
            /* 结束直播 */
            Singleton<LiveListMonitor>::getInstance()->remove(userId);
        }
    }
    break;
    case InfoType::NickName: {
        /* 更改昵称 */
        string userId = jsonMsg["userId"];
        string newNickName = jsonMsg["newNickName"];
        string command = "update User set nickName = '" + newNickName + "' where id =" + userId;
        Singleton<DbBroker>::getInstance()->query_execute(command);
    }
    break;
    case InfoType::VideoProfile: {
        /* 更改视频简介 */
        string videoId = jsonMsg["videoId"];
        string newProfile = jsonMsg["newProfile"];
        string command = "update Video set profile = '" + newProfile + "' where id =" + videoId;
        Singleton<DbBroker>::getInstance()->query_execute(command);
    }
    break;
    default:
        break;
    }
}

void ModifyInfoControl::modify_info(shared_ptr<tcp::socket> socket_ptr, const string &buf) {
    json jsonMsg = json::parse(buf);
    InfoType infotype = jsonMsg["infotype"];
    switch (infotype) {
    case InfoType::Comment: {
        /* 评论 */
        string id = std::to_string(Singleton<IdWorker>::getInstance()->nextId());
        string replyCommentId = jsonMsg["replyCommentId"];
        string publisherId = jsonMsg["publisherId"];
        string videoId = jsonMsg["videoId"];
        string content = jsonMsg["content"];
        /* 存入数据库 */
        string command = "insert into Comment(id, replyCommentId, publisherId, videoId, content, time) values('" +
                         id + "', '" + replyCommentId + "', '" + publisherId + "','" + videoId + "','" + content + "',(select now()))";
        Singleton<DbBroker>::getInstance()->query_execute(command);
    }
    break;
    case InfoType::Video: {
        string videoId = jsonMsg["videoId"];
        /* 删除视频文件 */
        string videoSuffix;
        string command = "select videoSuffix from Video where id=" + videoId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                auto it = res.begin();
                mysqlpp::Row row = *it;
                videoSuffix = string(row[0]);
            }
        }
        else cerr << "query.store() failed!" << endl;

        string dirpath = VIDEO_URL;
        string filepath = dirpath + videoId + videoSuffix;
        if (remove(filepath.data()) == 0) cout << "已删除视频文件" << videoId << endl;
                else cerr << "视频文件删除失败" << endl;
        /* 删除视频相关信息（视频信息+视频点赞信息） */
        //视频信息
        command = "delete from Video where id = " + videoId;
        if (Singleton<DbBroker>::getInstance()->query_execute(command)) cout << "已删除视频信息" << videoId << endl;
                else cerr << "视频删除失败" << endl;
        //视频点赞信息
        command = "delete from VideoLike where videoId = " + videoId;
        if (Singleton<DbBroker>::getInstance()->query_execute(command)) cout << "已删除视频点赞信息" << videoId << endl;
                else cerr << "视频点赞信息删除失败" << endl;
    }
    break;
    case InfoType::VideoLike: {
        /* 点赞(或取消点赞)视频 */
        string videoId = jsonMsg["id"];
        string userId = jsonMsg["userId"];
        bool ifLike = jsonMsg["ifLike"];
        if (ifLike) {
            /* 点赞 */
            string command = "insert into VideoLike values('" + videoId + "','" + userId + "')";
            Singleton<DbBroker>::getInstance()->query_execute(command);
        } else {
            /* 取消点赞 */
            string command = "delete from VideoLike where videoId="+videoId+" and userId="+userId;
            Singleton<DbBroker>::getInstance()->query_execute(command);
        }
    }
    break;
    case InfoType::CommentLike: {
        /* 点赞(或取消点赞)评论 */
        string commentId = jsonMsg["id"];
        string userId = jsonMsg["userId"];
        bool ifLike = jsonMsg["ifLike"];
        if (ifLike) {
            /* 点赞 */
            string command = "insert into CommentLike values('" + commentId + "','" + userId + "')";
            Singleton<DbBroker>::getInstance()->query_execute(command);
        } else {
            /* 取消点赞 */
            string command = "delete from CommentLike where videoId=" + commentId + " and userId=" + userId;
            Singleton<DbBroker>::getInstance()->query_execute(command);
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
            string command = "insert into Follow values('" + userId + "','" + followerId + "')";
            Singleton<DbBroker>::getInstance()->query_execute(command);
        } else {
            /* 取消关注某人 */
            string command = "delete from Follow where userId="+userId+" and followerId="+followerId;
            Singleton<DbBroker>::getInstance()->query_execute(command);
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
            Singleton<LiveListMonitor>::getInstance()->add(userId, url);

            json liveInfo;
            liveInfo["url"] = url;
            NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLiveInfo_P(liveInfo);
            boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
        } else {
            /* 结束直播 */
            Singleton<LiveListMonitor>::getInstance()->remove(userId);
        }
    }
    break;
    case InfoType::NickName: {
        /* 更改昵称 */
        string userId = jsonMsg["userId"];
        string newNickName = jsonMsg["newNickName"];
        string command = "update User set nickName = '" + newNickName + "' where id =" + userId;
        Singleton<DbBroker>::getInstance()->query_execute(command);
    }
    break;
    case InfoType::VideoProfile: {
        /* 更改视频简介 */
        string videoId = jsonMsg["videoId"];
        string newProfile = jsonMsg["newProfile"];
        string command = "update Video set profile = '" + newProfile + "' where id =" + videoId;
        Singleton<DbBroker>::getInstance()->query_execute(command);
    }
    break;
    default:
        break;
    }
}




