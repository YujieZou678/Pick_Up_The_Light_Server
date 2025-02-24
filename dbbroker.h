/*
func: 数据库经纪人，操作数据库。
author: zouyujie
date: 2024.12.2
*/
#ifndef DBBROKER_H
#define DBBROKER_H

#include <mysql++/mysql++.h>
#include <string>
using std::string;

#include "noncopyable.h"
#include "singleton.h"

class DbBroker : private Noncopyable
{
    friend class Singleton<DbBroker>;  //赋予单例类调用构造权限
public:
    void initDataBase();
    bool query_execute(const string &command);
    mysqlpp::StoreQueryResult query_store(const string &command);

private:
    DbBroker();

    mysqlpp::Connection m_conn;  //连接对象
};

#endif // DBBROKER_H
