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

class DbBroker
{
public:
    static DbBroker *getInstance();

    void initDataBase();
    bool query_execute(const string &command);
    mysqlpp::StoreQueryResult query_store(const string &command);

private:
    DbBroker();

    mysqlpp::Connection m_conn;  //连接对象
};

#endif // DBBROKER_H
