#include "sendinfocontrol.h"

#include <mysql++/mysql++.h>
#include <iostream>
using std::cout;
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
        string command = "select * from Comment where videoId="+videoId+" order by time desc";
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json comments;
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    comments.push_back(toJson_CommentInfo(string(row[0]), string(row[1]), string(row[2]), string(row[3]), string(row[4]), string(row[5])));
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
                    string url = NET_VIDEO_URL+string(row[0])+string(row[2]);
                    videoList.push_back(toJson_VodListInfo(string(row[0]), string(row[1]), url, string(row[3]), string(row[5])));
                }
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVodList_P(videoList);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
                /* 打印log */
                cout << "----VodList----" << endl;
                for (int i=0; i<videoList.size(); i++) {
                    json vodList = videoList[i];
                    cout << vodList["videoId"] << endl;
                    cout << vodList["publisherId"] << endl;
                    cout << vodList["url"] << endl;
                    cout << vodList["profile"] << endl;
                    cout << vodList["time"] << endl;
                    cout << "----end----" << endl;
                }
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
    case InfoType::VideoLike: {
        string videoId = jsonMsg["id"];
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
        string command = "select userId from Follow where followerId=" + userId;
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
        string command = "select followerId from Follow where userId=" + userId;
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
    case InfoType::NickName: {
        string userId = jsonMsg["userId"];
        string command = "select nickName from User where id=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json nickNameInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                nickNameInfo = toJson_NickNameInfo(userId, string(row[0]));
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendNickNameInfo_P(nickNameInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json nickNameInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendNickNameInfo_P(nickNameInfo);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::UserVideo: {
        string userId = jsonMsg["userId"];
        string command = "select id from Video where publisherId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json userVideoList;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    userVideoList.push_back(toJson_UserVideoInfo(string(row[0])));
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoInfo_P(userId, userVideoList);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json userVideoList;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoInfo_P(userId, userVideoList);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::UserVideoLike: {
        string userId = jsonMsg["userId"];
        string command = "select videoId from VideoLike where userId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json userVideoLikeList;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    userVideoLikeList.push_back(toJson_UserVideoInfo(string(row[0])));
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoLikeInfo_P(userId, userVideoLikeList);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json userVideoLikeList;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoLikeInfo_P(userId, userVideoLikeList);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Message: {
        string userId1 = jsonMsg["userId1"];
        string userId2 = jsonMsg["userId2"];
        string command = "select * from (select * from Message where sendId = "+ userId1 + " and receiveId = "+ userId2 +" union select * from Message where sendId = "+ userId2 + " and receiveId = "+ userId1+") as t order by time desc";
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json messages;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    messages.push_back(toJson_MessageInfo(string(row[0]), string(row[1]), string(row[2]), string(row[3]), string(row[4])));
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendMessageInfo_P(messages);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                json messages;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendMessageInfo_P(messages);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::VideoPush: {
        /* 获取视频推送数据，用完即删 */
        string userId = jsonMsg["userId"];
        string command = "select videoId from VideoPush where userId = " + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json videos;
                json video;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    video["videoId"] = string(row[0]);
                    /* 为每个视频查找对应的publisherId */
                    command = "select publisherId from Video where id = " + string(row[0]);
                    mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
                    if (res != NULL) {
                        if (res.begin() != res.end()) {
                            /* 有数据 */
                            auto data = res.begin();
                            row = *data;
                            video["publisherId"] = string(row[0]);
                        }
                    }
                    else cerr << "query.store() failed!" << endl;
                    /* 打包一个视频信息 */
                    videos.push_back(video);
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVideoPushInfo_P(videos);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
                /* 删除 */
                command = "delete from VideoPush where userId = " + userId;
                Singleton<DbBroker>::getInstance()->query_execute(command);
            } else {
                /* 没数据 */
                json videos;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVideoPushInfo_P(videos);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            }
        }
        else cerr << "query.store() failed!" << endl;
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
        string command = "select * from Comment where videoId="+videoId+" order by time desc";
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json comments;
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    comments.push_back(toJson_CommentInfo(string(row[0]), string(row[1]), string(row[2]), string(row[3]), string(row[4]), string(row[5])));
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
                /* 打印log */
                cout << "----VodList----" << endl;
                for (int i=0; i<videoList.size(); i++) {
                    json vodList = videoList[i];
                    cout << vodList["videoId"] << endl;
                    cout << vodList["publisherId"] << endl;
                    cout << vodList["url"] << endl;
                    cout << vodList["profile"] << endl;
                    cout << vodList["time"] << endl;
                    cout << "----end----" << endl;
                }
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
    case InfoType::VideoLike: {
        string videoId = jsonMsg["id"];
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
        string command = "select userId from Follow where followerId=" + userId;
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
        string command = "select followerId from Follow where userId=" + userId;
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
    case InfoType::NickName: {
        string userId = jsonMsg["userId"];
        string command = "select nickName from User where id=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json nickNameInfo;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                nickNameInfo = toJson_NickNameInfo(userId, string(row[0]));
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendNickNameInfo_P(nickNameInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json nickNameInfo;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendNickNameInfo_P(nickNameInfo);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::UserVideo: {
        string userId = jsonMsg["userId"];
        string command = "select id from Video where publisherId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json userVideoList;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    userVideoList.push_back(toJson_UserVideoInfo(string(row[0])));
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoInfo_P(userId, userVideoList);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json userVideoList;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoInfo_P(userId, userVideoList);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::UserVideoLike: {
        string userId = jsonMsg["userId"];
        string command = "select videoId from VideoLike where userId=" + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json userVideoLikeList;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    userVideoLikeList.push_back(toJson_UserVideoInfo(string(row[0])));
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoLikeInfo_P(userId, userVideoLikeList);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json userVideoLikeList;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendUserVideoLikeInfo_P(userId, userVideoLikeList);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::Message: {
        string userId1 = jsonMsg["userId1"];
        string userId2 = jsonMsg["userId2"];
        string command = "select * from (select * from Message where sendId = "+ userId1 + " and receiveId = "+ userId2 +" union select * from Message where sendId = "+ userId2 + " and receiveId = "+ userId1+") as t order by time desc";
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json messages;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    messages.push_back(toJson_MessageInfo(string(row[0]), string(row[1]), string(row[2]), string(row[3]), string(row[4])));
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendMessageInfo_P(messages);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            } else {
                /* 没数据 */
                json messages;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendMessageInfo_P(messages);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    case InfoType::VideoPush: {
        /* 获取视频推送数据，用完即删 */
        string userId = jsonMsg["userId"];
        string command = "select videoId from VideoPush where userId = " + userId;
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json videos;
                json video;
                auto it = res.begin();
                mysqlpp::Row row = *it;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    video["videoId"] = string(row[0]);
                    /* 为每个视频查找对应的publisherId */
                    command = "select publisherId from Video where id = " + string(row[0]);
                    mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
                    if (res != NULL) {
                        if (res.begin() != res.end()) {
                            /* 有数据 */
                            auto data = res.begin();
                            row = *data;
                            video["publisherId"] = string(row[0]);
                        }
                    }
                    else cerr << "query.store() failed!" << endl;
                    /* 打包一个视频信息 */
                    videos.push_back(video);
                }

                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVideoPushInfo_P(videos);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
                /* 删除 */
                command = "delete from VideoPush where userId = " + userId;
                Singleton<DbBroker>::getInstance()->query_execute(command);
            } else {
                /* 没数据 */
                json videos;
                NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVideoPushInfo_P(videos);
                boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
            }
        }
        else cerr << "query.store() failed!" << endl;
    }
    break;
    default:
        break;
    }
}

json SendInfoControl::toJson_CommentInfo(const string &id, const string &replyCommentId, const string &publisherId, const string &videoId, const string &content, const string &time)
{
    json commentInfo;
    commentInfo["id"] = id;
    commentInfo["replyCommentId"] = replyCommentId;
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

json SendInfoControl::toJson_NickNameInfo(const string &userId, const string &nickName)
{
    json nickNameInfo;
    nickNameInfo["userId"] = userId;
    nickNameInfo["nickName"] = nickName;
    return nickNameInfo;
}

json SendInfoControl::toJson_UserVideoInfo(const string &videoId)
{
    json userVideoInfo;
    userVideoInfo["videoId"] = videoId;
    return userVideoInfo;
}

json SendInfoControl::toJson_MessageInfo(const string &sendId, const string &receiveId, const string &messageType, const string &content, const string &time)
{
    json messageInfo;
    messageInfo["sendId"] = sendId;
    messageInfo["receiveId"] = receiveId;
    messageInfo["messageType"] = messageType;
    messageInfo["content"] = content;
    messageInfo["time"] = time;
    return messageInfo;
}










