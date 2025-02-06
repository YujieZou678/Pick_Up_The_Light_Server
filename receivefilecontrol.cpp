#include "receivefilecontrol.h"

#include <fstream>
using std::ofstream;

#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "dbbroker.h"
#include "snowflakeidworker.h"
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
    string dirpath, filepath, command;
    if (jsonMsg["filetype"] == FileType::ProfilePicture) {
        /* 头像文件 */
        dirpath = PROFILE_PICTURE_URL;
        filepath = dirpath + string(jsonMsg["id"]) + string(jsonMsg["suffix"]);
        command = "update User set pictureSuffix='" + string(jsonMsg["suffix"]) + "' where id=" + string(jsonMsg["id"]);
    } else if (jsonMsg["filetype"] == FileType::Video) {
        /* 视频文件 */
        string videoId = std::to_string(Singleton<IdWorker>::instance().nextId());
        dirpath = VIDEO_URL;
        filepath = dirpath + videoId + string(jsonMsg["suffix"]);
        command = "insert into Video(id, publisherId, videoSuffix, profile, time) values('" + videoId + "', '" + string(jsonMsg["id"]) + "', '" + string(jsonMsg["suffix"]) + "', '" + string(jsonMsg["profile"]) + "',(select now()))";
    }

    /* 处理文件数据 */
    ofstream ofs(filepath);  //覆盖写入
    ofs.write(fileData, size);
    ofs.close();  //关闭文件
    safe_delete_arr(fileData);  //释放内存

    /* 更新数据库 */
    DbBroker::getInstance()->query_execute(command);

    return true;
}











