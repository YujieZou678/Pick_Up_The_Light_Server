/*
func: 数据库类，定义了数据库的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <mysql++/mysql++.h>

class MyDataBase
{
public:
    MyDataBase(const char*, const char*);
    ~MyDataBase();
    bool check_ID(const char*);  //检查账户是否存在
    bool do_register(const char*, const char*);
    bool do_login();
    bool get_info_vod();
    bool add_info_vod();
    bool update_info_vod();
    bool remove_info_vod();

private:
    mysqlpp::Connection conn{false};  //连接对象
    mysqlpp::Query query{conn.query()};  //执行对象
    mysqlpp::StoreQueryResult res;  //得到结果集
    mysqlpp::StoreQueryResult::const_iterator it;
    mysqlpp::Row row;
};

#endif // MYDATABASE_H
