#include "sendinfocontrol.h"

#include <mysql++/mysql++.h>
#include <iostream>
using std::cerr;
using std::endl;

#include "dbbroker.h"
#include "netpacketgenerator.h"
#include "livelistmonitor.h"
#include "config.h"

SendInfoControl::SendInfoControl()
{
}

void SendInfoControl::send_info(int fd, const string &buf)
{
    json jsonMsg = json::parse(buf);
    InfoType infotype = jsonMsg["infotype"];
    switch (infotype) {
    case InfoType::Comment: {
        /* 获取评论 */
        string videoId = jsonMsg["id"];
        string command = "select * from Comment where videoId="+videoId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json comments;
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    comments.push_back(toJson_CommentInfo(string(row[0]), string(row[1]), string(row[2]), string(row[3])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendComments_P(comments);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json comments;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendComments_P(comments);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::VodList: {
        /* 获取点播视频列表 */
        int offset = jsonMsg["offset"];
        int num = jsonMsg["num"];
        string command = "select * from Video limit " + std::to_string(offset) + "," + std::to_string(num);
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json videoList;
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    string url = "http://127.0.0.1/"+string(row[0])+string(row[2]);
                    videoList.push_back(toJson_VodListInfo(string(row[0]), string(row[1]), url, string(row[3]), string(row[5])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVodList_P(videoList);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json videoList;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVodList_P(videoList);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Like: {
        string videoId = jsonMsg["videoId"];
        string userId = jsonMsg["userId"];
        string command = "select count(distinct userId),(select if(count(*)=1,true,false) from VideoLike where videoId="+videoId+" and userId="+userId+") from VideoLike where videoId="+videoId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json likeInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                likeInfo = toJson_LikeInfo(row[0], row[1]);
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLikeInfo_P(likeInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json likeInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLikeInfo_P(likeInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Follow: {
        string userId = jsonMsg["userId"];
        string command = "select userId from Attention where followerId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json followInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    followInfo.push_back(toJson_FollowInfo(string(row[0])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFollowInfo_P(followInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json followInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFollowInfo_P(followInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Fans: {
        string userId = jsonMsg["userId"];
        string command = "select followerId from Attention where userId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json fansInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    fansInfo.push_back(toJson_FansInfo(string(row[0])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFansInfo_P(fansInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json fansInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFansInfo_P(fansInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::LiveList: {
        json liveListInfo;
        unordered_map<string,string> liveList = Singleton<LiveListMonitor>::getInstance()->getLiveList();
        for (auto it : liveList) {
            liveListInfo.push_back(toJson_LiveListInfo(it.first, it.second));
        }
        NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLiveList_P(liveListInfo);
        my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
    }
    break;
    default:
        break;
    }
}

void SendInfoControl::send_info(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const string &buf)
{
    json jsonMsg = json::parse(buf);
    InfoType infotype = jsonMsg["infotype"];
    switch (infotype) {
    case InfoType::Comment: {
        /* 获取评论 */
        string videoId = jsonMsg["id"];
        string command = "select * from Comment where videoId="+videoId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json comments;
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    comments.push_back(toJson_CommentInfo(string(row[0]), string(row[1]), string(row[2]), string(row[3])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendComments_P(comments);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json comments;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendComments_P(comments);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::VodList: {
        /* 获取点播视频列表 */
        int offset = jsonMsg["offset"];
        int num = jsonMsg["num"];
        string command = "select * from Video limit " + std::to_string(offset) + "," + std::to_string(num);
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json videoList;
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    string url = "http://127.0.0.1/"+string(row[0])+string(row[2]);
                    videoList.push_back(toJson_VodListInfo(string(row[0]), string(row[1]), url, string(row[3]), string(row[5])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVodList_P(videoList);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json videoList;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVodList_P(videoList);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Like: {
        string videoId = jsonMsg["videoId"];
        string userId = jsonMsg["userId"];
        string command = "select count(distinct userId),(select if(count(*)=1,true,false) from VideoLike where videoId="+videoId+" and userId="+userId+") from VideoLike where videoId="+videoId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json likeInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                likeInfo = toJson_LikeInfo(row[0], row[1]);
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLikeInfo_P(likeInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json likeInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLikeInfo_P(likeInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Follow: {
        string userId = jsonMsg["userId"];
        string command = "select userId from Attention where followerId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json followInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    followInfo.push_back(toJson_FollowInfo(string(row[0])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFollowInfo_P(followInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json followInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFollowInfo_P(followInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Fans: {
        string userId = jsonMsg["userId"];
        string command = "select followerId from Attention where userId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json fansInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    fansInfo.push_back(toJson_FansInfo(string(row[0])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFansInfo_P(fansInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json fansInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendFansInfo_P(fansInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::LiveList: {
        json liveListInfo;
        unordered_map<string,string> liveList = Singleton<LiveListMonitor>::getInstance()->getLiveList();
        for (auto it : liveList) {
            liveListInfo.push_back(toJson_LiveListInfo(it.first, it.second));
        }
        NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendLiveList_P(liveListInfo);
        boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
    }
    break;
    default:
        break;
    }
}

json SendInfoControl::toJson_CommentInfo(const string &publisherId, const string &videoId, const string &content, const string &time)
{
    json commentInfo;
    commentInfo["publisherId"] = publisherId;
    commentInfo["videoId"] = videoId;
    commentInfo["content"] = content;
    commentInfo["time"] = time;
    return commentInfo;
}

json SendInfoControl::toJson_VodListInfo(const string &videoId, const string &publisherId, const string &url, const string &profile, const string &time)
{
    json vodListInfo;
    vodListInfo["videoId"] = videoId;
    vodListInfo["publisherId"] = publisherId;
    vodListInfo["url"] = url;
    vodListInfo["profile"] = profile;
    vodListInfo["time"] = time;
    return vodListInfo;
}

json SendInfoControl::toJson_LikeInfo(int likeCount, bool ifLike)
{
    json likeInfo;
    likeInfo["likeCount"] = likeCount;
    likeInfo["ifLike"] = ifLike;
    return likeInfo;
}

json SendInfoControl::toJson_FollowInfo(const string &userId)
{
    json followInfo;
    followInfo["userId"] = userId;
    return followInfo;
}

json SendInfoControl::toJson_FansInfo(const string &followerId)
{
    json fansInfo;
    fansInfo["followerId"] = followerId;
    return fansInfo;
}

json SendInfoControl::toJson_LiveListInfo(const string &publisherId, const string &url)
{
    json liveListInfo;
    liveListInfo["publisherId"] = publisherId;
    liveListInfo["url"] = url;
    return liveListInfo;
}










