#include "sendinfocontrol.h"

#include <mysql++/mysql++.h>
#include <iostream>
using std::cerr;
using std::endl;

#include "dbbroker.h"
#include "netpacketgenerator.h"
#include "config.h"

SendInfoControl::SendInfoControl()
{
}

SendInfoControl *SendInfoControl::getInstance()
{
    static SendInfoControl instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
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
        mysqlpp::StoreQueryResult res = DbBroker::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                json comments;
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    comments.push_back(toJson_CommentInfo(string(row[0]), string(row[1]), string(row[2]), string(row[3])));
                }
                NetPacket p = NetPacketGenerator::getInstance()->sendComments_P(comments);
                my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
            } else {
                /* 没数据 */
                //??
            }
        }
        else cerr << "query.store() failed!" << endl;
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
