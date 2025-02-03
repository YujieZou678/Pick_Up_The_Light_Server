/*
func: 发送信息控制类。
author: zouyujie
date: 2025.2.2
*/
#ifndef SENDINFOCONTROL_H
#define SENDINFOCONTROL_H

#include <string>
using std::string;
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class SendInfoControl
{
public:
    static SendInfoControl *getInstance();
    void send_info(int fd, const string &buf);
    json toJson_CommentInfo(const string &publisherId, const string &videoId, const string &content, const string &time);

private:
    SendInfoControl();
};

#endif // SENDINFOCONTROL_H
