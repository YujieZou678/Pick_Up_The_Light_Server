#include "sendfilecontrol.h"

#include <sys/stat.h>
#include <fstream>
using std::ifstream;
#include <iostream>
using std::cout;
using std::endl;
#include <string.h>

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

json SendFileControl::toJson_FileInfo(const FileType &filetype, const string &id, const string &suffix)
{
    json fileInfo;
    fileInfo["filetype"] = filetype;
    fileInfo["id"] = id;
    fileInfo["suffix"] = suffix;
    return fileInfo;
}

bool SendFileControl::send_file(int fd, const FileType &filetype, const char *filepath, const char *suffix)
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
    json json_fileInfo = toJson_FileInfo(filetype, "2894841947", suffix);
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
    string dirpath, filepath;
    if (jsonMsg["filetype"] == FileType::ProfilePicture) {
        /* 头像文件 */
        dirpath = PROFILE_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + ".png";
        if (send_file(fd, FileType::ProfilePicture, filepath.data(), ".png")) {
            std::cout << "发送成功" << std::endl;
        } else std::cout << "发送失败" << std::endl;
    }
}
