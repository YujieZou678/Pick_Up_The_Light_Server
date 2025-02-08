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

private:
    SendInfoControl();
    json toJson_CommentInfo(const string &publisherId, const string &videoId, const string &content, const string &time);
    json toJson_VodListInfo(const string &videoId, const string &publisherId, const string &url, const string &profile, const string &time);
    json toJson_LikeInfo(int likeCount, bool ifLike);
    json toJson_FollowInfo(const string &userId);
    json toJson_FansInfo(const string &followerId);
    json toJson_LiveListInfo(const string &publisherId, const string &url);
};

#endif // SENDINFOCONTROL_H
