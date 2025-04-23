#include "receivefilecontrol.h"

#include <fstream>
using std::ofstream;

#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "dbbroker.h"
#include "netpacketgenerator.h"
#include "snowflakeidworker.h"
#include "config.h"

ReceiveFileControl::ReceiveFileControl()
{
}

bool ReceiveFileControl::receive_file(int fd, const string &fileInfo, char *fileData, size_t size)
{
    /* 处理文件信息 */
    json jsonMsg = json::parse(fileInfo);
    string dirpath, filepath, command;
    string videoId;

    if (jsonMsg["filetype"] == FileType::ProfilePicture) {
        /* 头像文件 */
        dirpath = PROFILE_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + string(jsonMsg["suffix"]);
        command = "update User set pictureSuffix='" + string(jsonMsg["suffix"]) + "' where id='" + string(jsonMsg["id"])+"'";
    } else if (jsonMsg["filetype"] == FileType::ChatImg) {
        dirpath = CHAT_PICTURE_URL;
        /* 为聊天图像随机生成个不重复的名字 */
        string chatImgId = std::to_string(Singleton<IdWorker>::getInstance()->nextId());
        filepath = dirpath + chatImgId + string(jsonMsg["suffix"]);
        command = "insert into Message(sendId, receiveId, messageType, message, time) values('"+ string(jsonMsg["sendId"]) +"', '" + string(jsonMsg["receiveId"]) + "', '" + std::to_string(int(MessageType::ChatImg)) + "', '"+ chatImgId + string(jsonMsg["suffix"]) +"', (select now()))";
    } else if (jsonMsg["filetype"] == FileType::VideoPreviewImg) {
        dirpath = PREVIEW_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + string(jsonMsg["suffix"]);
        command = "update Video set previewSuffix = '" + string(jsonMsg["suffix"]) + "' where id = '" + string(jsonMsg["id"]) + "'";
    } else if (jsonMsg["filetype"] == FileType::Video) {
        /* 视频文件 */
        videoId = std::to_string(Singleton<IdWorker>::getInstance()->nextId());
        dirpath = VIDEO_URL;
        filepath = dirpath + videoId + string(jsonMsg["suffix"]);
        command = "insert into Video(id, publisherId, videoSuffix, profile, time) values('" + videoId + "', '" + string(jsonMsg["id"]) + "', '" + string(jsonMsg["suffix"]) + "', '" + string(jsonMsg["profile"]) + "',(select now()))";
        /* videoId传回客户端 */
        NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVideoId_P(videoId);
        my_send(fd, &p, sizeof(NetPacketHeader)+p.packetHeader.data_size, 0);
    }

    /* 处理文件数据 */
    ofstream ofs(filepath);  //覆盖写入
    ofs.write(fileData, size);
    ofs.close();  //关闭文件
    safe_delete_arr(fileData);  //释放内存

    /* 更新数据库 */
    Singleton<DbBroker>::getInstance()->query_execute(command);
    if (jsonMsg["filetype"] == FileType::Video) {
        /* 添加新视频推送功能 */
        command = "select followerId from Follow where userId = " + string(jsonMsg["id"]);
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    command = "insert into VideoPush Values('"+ string(row[0]) +"', '"+ videoId +"')";
                    Singleton<DbBroker>::getInstance()->query_execute(command);
                }
            }
        }
        else std::cerr << "query.store() failed!" << std::endl;
    }

    return true;
}

bool ReceiveFileControl::receive_file(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const string &fileInfo, char *fileData, size_t size)
{
    /* 处理文件信息 */
    json jsonMsg = json::parse(fileInfo);
    string dirpath, filepath, command;
    string videoId;

    if (jsonMsg["filetype"] == FileType::ProfilePicture) {
        /* 头像文件 */
        dirpath = PROFILE_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + string(jsonMsg["suffix"]);
        command = "update User set pictureSuffix='" + string(jsonMsg["suffix"]) + "' where id=" + string(jsonMsg["id"]);
    } else if (jsonMsg["filetype"] == FileType::ChatImg) {
        dirpath = CHAT_PICTURE_URL;
        /* 为聊天图像随机生成个不重复的名字 */
        string chatImgId = std::to_string(Singleton<IdWorker>::getInstance()->nextId());
        filepath = dirpath + chatImgId + string(jsonMsg["suffix"]);
        command = "insert into Message(sendId, receiveId, messageType, message, time) values('"+ string(jsonMsg["sendId"]) +"', '" + string(jsonMsg["receiveId"]) + "', '" + std::to_string(int(MessageType::ChatImg)) + "', '"+ chatImgId + string(jsonMsg["suffix"]) +"', (select now()))";
    } else if (jsonMsg["filetype"] == FileType::VideoPreviewImg) {
        dirpath = PREVIEW_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + string(jsonMsg["suffix"]);
        command = "update Video set previewSuffix = '" + string(jsonMsg["suffix"]) + "' where id = '" + string(jsonMsg["id"]) + "'";
    } else if (jsonMsg["filetype"] == FileType::Video) {
        /* 视频文件 */
        videoId = std::to_string(Singleton<IdWorker>::getInstance()->nextId());
        dirpath = VIDEO_URL;
        filepath = dirpath + videoId + string(jsonMsg["suffix"]);
        command = "insert into Video(id, publisherId, videoSuffix, profile, time) values('" + videoId + "', '" + string(jsonMsg["id"]) + "', '" + string(jsonMsg["suffix"]) + "', '" + string(jsonMsg["profile"]) + "',(select now()))";
        /* videoId传回客户端 */
        NetPacket p = Singleton<NetPacketGenerator>::getInstance()->sendVideoId_P(videoId);
        boost::asio::write(*socket_ptr, boost::asio::buffer(&p, sizeof(NetPacketHeader)+p.packetHeader.data_size));
    }

    /* 处理文件数据 */
    ofstream ofs(filepath);  //覆盖写入
    ofs.write(fileData, size);
    ofs.close();  //关闭文件
    safe_delete_arr(fileData);  //释放内存

    /* 更新数据库 */
    Singleton<DbBroker>::getInstance()->query_execute(command);
    if (jsonMsg["filetype"] == FileType::Video) {
        /* 添加新视频推送功能 */
        command = "select followerId from Follow where userId = " + string(jsonMsg["id"]);
        mysqlpp::StoreQueryResult res = Singleton<DbBroker>::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                /* 有数据 */
                mysqlpp::Row row;
                for (auto it = res.begin(); it < res.end(); it++) {
                    row = *it;
                    command = "insert into VideoPush Values('"+ string(row[0]) +"', '"+ videoId +"')";
                    Singleton<DbBroker>::getInstance()->query_execute(command);
                }
            }
        }
        else std::cerr << "query.store() failed!" << std::endl;
    }

    return true;
}











