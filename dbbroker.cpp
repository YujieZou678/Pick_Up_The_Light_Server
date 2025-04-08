/*
func: 数据库经纪人的实现。
author: zouyujie
date: 2024.12.2
*/
#include "dbbroker.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

DbBroker::DbBroker() :
    m_conn(false)
{
    if (m_conn.connect("Server", "localhost", "root", "")) {
        std::cout << "DB: Successfully connect!" << std::endl;
    }
    else {
        perror("DB: connection failed");
        exit(1);
    }
}

//DbBroker::~DbBroker()
//{
//    conn.disconnect();
//    std::cout << std::endl;
//    std::cout << "Successfully drop the connection!" << std::endl;
//}

void DbBroker::initDataBase()
{
    string command;
    /*
     * 点播视频信息
     * id publisherId videoSuffix previewSuffix profile time
     * 创建表Video */
    command = "create table Video(id varchar(30), publisherId varchar(30), videoSuffix varchar(20), profile varchar(500), previewSuffix varchar(20), time datetime)";
    query_execute(command);
    /*
     * 用户信息
     * id pw nickName pictureSuffix
     * 创建表User */
    command = "create table User(id varchar(30), pw varchar(30), nickName varchar(30), pictureSuffix varchar(20))";
    query_execute(command);
    /*
     * 聊天记录
     * sendId receiveId messageType message time
     * 创建表Message */
    command = "create table Message(sendId varchar(30), receiveId varchar(30), messageType int, message varchar(500), time datetime)";
    query_execute(command);
    /*
     * 评论
     * id replyCommentId videoId content publisherId time
     * 创建表Comment */
    command = "create table Comment(id varchar(30), replyCommentId varchar(30), publisherId varchar(30), videoId varchar(30), content varchar(500), time datetime)";
    query_execute(command);
    /*
     * 点播视频点赞
     * videoId userId
     * 创建表VideoLike */
    command = "create table VideoLike(videoId varchar(30), userId varchar(30))";
    query_execute(command);
    /*
     * 评论点赞
     * commentId userId
     * 创建表CommentLike */
    command = "create table CommentLike(commentId varchar(30), userId varchar(30))";
    query_execute(command);
    /*
     * 关注
     * userId followerId
     * 创建表Follow */
    command = "create table Follow(userId varchar(30), followerId varchar(30))";
    query_execute(command);

    cout << "DB: Successfully init!" << endl;
}

bool DbBroker::query_execute(const string &command)
{
    mysqlpp::Query query = m_conn.query(command);  //执行对象
    if (query.execute()) return true;
    cerr << "query.execute() failed: " << query.error() << endl;
    return false;
}

mysqlpp::StoreQueryResult DbBroker::query_store(const string &command)
{
    mysqlpp::Query query = m_conn.query(command);  //执行对象
    mysqlpp::StoreQueryResult res;  //结果集
    res = query.store();
    return res;
}


