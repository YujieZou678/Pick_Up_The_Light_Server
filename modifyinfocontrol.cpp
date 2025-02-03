#include "modifyinfocontrol.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <iostream>

#include "dbbroker.h"
#include "config.h"

ModifyInfoControl::ModifyInfoControl()
{
}

ModifyInfoControl *ModifyInfoControl::getInstance()
{
    static ModifyInfoControl instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

void ModifyInfoControl::modify_info(const string &buf)
{
    json jsonMsg = json::parse(buf);
    if (jsonMsg["infotype"] == InfoType::Comment) {
        /* 评论 */
        string publisherId = jsonMsg["publisherId"];
        string videoId = jsonMsg["videoId"];
        string content = jsonMsg["content"];
        /* 存入数据库 */
        string command = "insert into Comment(publisherId, videoId, content, time) values('" +
                         publisherId + "','" + videoId + "','" + content + "',(select now()))";
        DbBroker::getInstance()->query_execute(command);
    }
}




