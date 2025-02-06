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

DbBroker *DbBroker::getInstance()
{
    static DbBroker instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

void DbBroker::initDataBase()
{
    string command;
    /* 创建表User */
    command = "create table User(id varchar(30), pw varchar(30), nickName varchar(30), pictureSuffix varchar(20))";
    query_execute(command);
    /* 创建表Comment */
    command = "create table Comment(publisherId varchar(30), videoId varchar(30), content varchar(500), time datetime)";
    query_execute(command);
    /* 创建表Video */
    command = "create table Video(id varchar(30), publisherId varchar(30), videoSuffix varchar(20), profile varchar(500), previewSuffix varchar(20), time datetime)";
    query_execute(command);
    /* 创建表VideoLike */
    command = "create table VideoLike(videoId varchar(30), userId varchar(30))";
    query_execute(command);
    /* 创建表Attention */
    command = "create table Attention(userId varchar(30), followerId varchar(30))";
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


