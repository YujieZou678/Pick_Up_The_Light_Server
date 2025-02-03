#include "receivefilecontrol.h"

#include <fstream>
using std::ofstream;

#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "config.h"

ReceiveFileControl::ReceiveFileControl()
{
}

ReceiveFileControl *ReceiveFileControl::getInstance()
{
    static ReceiveFileControl instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

bool ReceiveFileControl::receive_file(const string &fileInfo, char *fileData, size_t size)
{
    /* 处理文件信息 */
    json jsonMsg = json::parse(fileInfo);
    string dirpath, filepath;
    if (jsonMsg["filetype"] == FileType::ProfilePicture) {
        /* 头像文件 */
        dirpath = PROFILE_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + ".png";
    }

    /* 处理文件数据 */
    ofstream ofs(filepath);  //覆盖写入
    ofs.write(fileData, size);
    ofs.close();  //关闭文件
    safe_delete_arr(fileData);  //释放内存

    return true;
}











