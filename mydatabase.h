/*
func: 数据库类，定义了数据库的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef MYDATABASE_H
#define MYDATABASE_H


class MyDataBase
{
public:
    MyDataBase(const char*, const char*);
    ~MyDataBase();
    bool do_register();
    bool do_login();
    bool get_info_vod();
    bool add_info_vod();
    bool update_info_vod();
    bool remove_info_vod();

private:
    const char* accountNumber;
    const char* password;
};

#endif // MYDATABASE_H
