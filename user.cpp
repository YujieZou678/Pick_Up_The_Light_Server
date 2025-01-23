#include "user.h"

User::User(const string &id, const string &pw, const string &nickName) :
    UserInterface(id),
    m_pw(pw),
    m_nickName(nickName)
{
}

json User::getInfo()
{
    json userInfo;
    userInfo["pw"] = m_pw;
    userInfo["nickName"] = m_nickName;
    return userInfo;
}

string User::getPw() const
{
    return m_pw;
}

string User::getNickName() const
{
    return m_nickName;
}
