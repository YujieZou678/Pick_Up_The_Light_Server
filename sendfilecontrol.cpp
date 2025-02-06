#include "sendfilecontrol.h"

#include <sys/stat.h>
#include <fstream>
using std::ifstream;
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <string.h>

#include "dbbroker.h"

SendFileControl::SendFileControl()
{
}

SendFileControl *SendFileControl::getInstance()
{
    static SendFileControl instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

size_t SendFileControl::getFileSize(const char *fileName)
{
    if (fileName == nullptr) {
        return 0;
    }
    struct stat statbuf;
    if (stat(fileName, &statbuf) != 0) {
        perror("stat error");
        return 0;
    }
    size_t filesize = statbuf.st_size;  //获取文件大小

    return filesize;
}

json SendFileControl::toJson_PFileInfo(const FileType &filetype, const string &id, const string &suffix)
{
    json pfileInfo;
    pfileInfo["filetype"] = filetype;
    pfileInfo["id"] = id;
    pfileInfo["suffix"] = suffix;
    return pfileInfo;
}

json SendFileControl::toJson_VFileInfo(const FileType &filetype, const string &videoId, const string &publisherId, const string &suffix)
{
    json vfileInfo;
    vfileInfo["filetype"] = filetype;
    vfileInfo["videoId"] = videoId;
    vfileInfo["publisherId"] = publisherId;
    vfileInfo["suffix"] = suffix;
    return vfileInfo;
}

bool SendFileControl::send_pfile(int fd, const FileType &filetype, const char *filepath, const string &id, const string &suffix)
{
    /* 检测大小 */
    unsigned int file_size = getFileSize(filepath);
    if (file_size == 0) {
        /* 没有该文件 */
        NetPacket p;
        p.packetHeader.purpose = Purpose::GetFile;
        p.packetHeader.data_size = 0;
        my_send(fd, &p, sizeof(NetPacketHeader), 0);
        return false;
    }

    ifstream ifs(filepath, std::ios::in|std::ios::binary);  //读二进制文件
    if (!ifs.is_open()) {
        perror("ifstream error");
        return false;
    }

    FileNetPacket *p = new FileNetPacket;  //文件网络包,堆内存
    /* 包头初始化 */
    p->packetHeader.purpose = Purpose::GetFile;
    p->packetHeader.data_size = file_size;
    /* 文件数据包初始化 */
    json json_fileInfo = toJson_PFileInfo(filetype, id, suffix);
    string str_fileInfo = json_fileInfo.dump();
    strcpy(p->fileDataPacket.fileInfo, str_fileInfo.data());  //文件信息
    ifs.read(p->fileDataPacket.fileData, file_size);  //文件数据
    ifs.close();  //关闭文件

    int ret = 0;
    ret = my_send(fd, p, sizeof(NetPacketHeader)+BUF_SIZE+file_size, 0);
    safe_delete(p);  //释放内存

    cout << "实际发送:" << ret << endl;
    if (ret == 0 || ret == -1) return false;
    return true;
}

bool SendFileControl::send_vfile(int fd, const FileType &filetype, const char *filepath, const string &videoId, const string &publisherId, const string &suffix)
{
    /* 检测大小 */
    unsigned int file_size = getFileSize(filepath);
    if (file_size == 0) {
        /* 没有该文件 */
        NetPacket p;
        p.packetHeader.purpose = Purpose::GetFile;
        p.packetHeader.data_size = 0;
        my_send(fd, &p, sizeof(NetPacketHeader), 0);
        return false;
    }

    ifstream ifs(filepath, std::ios::in|std::ios::binary);  //读二进制文件
    if (!ifs.is_open()) {
        perror("ifstream error");
        return false;
    }

    FileNetPacket *p = new FileNetPacket;  //文件网络包,堆内存
    /* 包头初始化 */
    p->packetHeader.purpose = Purpose::GetFile;
    p->packetHeader.data_size = file_size;
    /* 文件数据包初始化 */
    json json_fileInfo = toJson_VFileInfo(filetype, videoId, publisherId, suffix);
    string str_fileInfo = json_fileInfo.dump();
    strcpy(p->fileDataPacket.fileInfo, str_fileInfo.data());  //文件信息
    ifs.read(p->fileDataPacket.fileData, file_size);  //文件数据
    ifs.close();  //关闭文件

    int ret = 0;
    ret = my_send(fd, p, sizeof(NetPacketHeader)+BUF_SIZE+file_size, 0);
    safe_delete(p);  //释放内存

    cout << "实际发送:" << ret << endl;
    if (ret == 0 || ret == -1) return false;
    return true;
}

void SendFileControl::send_file(int fd, const string &buf)
{
    json jsonMsg = json::parse(buf);
    string dirpath, filepath, command;
    if (jsonMsg["filetype"] == FileType::ProfilePicture) {
        /* 头像文件 */
        string pictureSuffix = "";
        command = "select pictureSuffix from User where id=" + string(jsonMsg["id"]);
        mysqlpp::StoreQueryResult res = DbBroker::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                auto it = res.begin();
                mysqlpp::Row row = *it;
                pictureSuffix = string(row[0]);
            }
        }
        else cerr << "query.store() failed!" << endl;

        dirpath = PROFILE_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + pictureSuffix;
        if (send_pfile(fd, FileType::ProfilePicture, filepath.data(), string(jsonMsg["id"]), pictureSuffix)) {
            std::cout << "发送成功" << std::endl;
        } else std::cout << "发送失败" << std::endl;
    } else if (jsonMsg["filetype"] == FileType::Video) {
        /* 点播视频文件 */
        string videoSuffix = "", publisherId = "";
        command = "select videoSuffix,publisherId from Video where id=" + string(jsonMsg["id"]);
        mysqlpp::StoreQueryResult res = DbBroker::getInstance()->query_store(command);
        if (res != NULL) {
            if (res.begin() != res.end()) {
                auto it = res.begin();
                mysqlpp::Row row = *it;
                videoSuffix = string(row[0]);
                publisherId = string(row[1]);
            }
        }
        else cerr << "query.store() failed!" << endl;

        dirpath = VIDEO_URL;
        filepath = dirpath + string(jsonMsg["id"]) + videoSuffix;
        if (send_vfile(fd, FileType::Video, filepath.data(), string(jsonMsg["id"]), publisherId, videoSuffix)) {
            std::cout << "发送成功" << std::endl;
        } else std::cout << "发送失败" << std::endl;
    }
}
