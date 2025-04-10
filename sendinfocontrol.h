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
#include <boost/asio.hpp>
#include <memory>
using std::shared_ptr;

#include "noncopyable.h"
#include "singleton.h"

class SendInfoControl : private Noncopyable
{
    friend class Singleton<SendInfoControl>;  //赋予单例类调用构造权限
public:
    void send_info(int fd, const string &buf);
    void send_info(shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const string &buf);  //重载

private:
    SendInfoControl();
    json toJson_CommentInfo(const string &id, const string &replyCommentId, const string &publisherId, const string &videoId, const string &content, const string &time);
    json toJson_VodListInfo(const string &videoId, const string &publisherId, const string &url, const string &profile, const string &time);
    json toJson_LikeInfo(int likeCount, bool ifLike);
    json toJson_FollowInfo(const string &userId);
    json toJson_FansInfo(const string &followerId);
    json toJson_LiveListInfo(const string &publisherId, const string &url);
    json toJson_NickNameInfo(const string &userId, const string &nickName);
    json toJson_UserVideoInfo(const string &videoId);
    json toJson_MessageInfo(const string &sendId, const string &receiveId, const string &messageType, const string &content, const string &time);
};

#endif // SENDINFOCONTROL_H
