#include "comment.h"

Comment::Comment(const string &content, const string &time,
                 const string &publisherId, const string &videoId) :
    m_content(content),
    m_time(time),
    m_publisherId(publisherId),
    m_videoId(videoId)
{
}

json Comment::getInfo()
{
    json commentInfo;
    commentInfo["content"] = m_content;
    commentInfo["time"] = m_time;
    commentInfo["publisherId"] = m_publisherId;
    commentInfo["videoId"] = m_videoId;
    return commentInfo;
}

string Comment::getContent() const
{
    return m_content;
}

string Comment::getTime() const
{
    return m_time;
}

string Comment::getPublisherId() const
{
    return m_publisherId;
}

string Comment::getVideoId() const
{
    return m_videoId;
}

