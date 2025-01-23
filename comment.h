/*
func: 评论实体类。
author: zouyujie
date: 2025.1.19
*/
#ifndef COMMENT_H
#define COMMENT_H

#include <string>
using std::string;

#include "commentinterface.h"

class Comment : public CommentInterface
{
public:
    Comment(const string &content, const string &time,
            const string &publisherId, const string &videoId);
    ~Comment() {}

    json getInfo() override;

    string getContent() const;
    string getTime() const;
    string getPublisherId() const;
    string getVideoId() const;

private:
    string m_content;
    string m_time;
    string m_publisherId;
    string m_videoId;
};

#endif // COMMENT_H
