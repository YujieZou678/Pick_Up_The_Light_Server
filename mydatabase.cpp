/*
func: 数据库类的实现。
author: zouyujie
date: 2024.12.2
*/
#include "mydatabase.h"

#include <string>
using std::string;

MyDataBase::MyDataBase(const char* accountNumber, const char* password)
{
    if (conn.connect("Server", "localhost", accountNumber, password)) {
        std::cout << "DB Database: Successfully connect!" << std::endl;
    }
    else {
        perror("DB Database: connection failed");
        exit(1);
    }
}

MyDataBase::~MyDataBase()
{
    conn.disconnect();
    std::cout << std::endl;
    std::cout << "Successfully drop the connection!" << std::endl;
}

bool MyDataBase::check_ID(const char *id)
{
    query.reset();
    query << "select ID from id_pw where ID = " << id;
    res = query.store();

    if (res != NULL) {
        if (res.begin() == res.end()) return false;
        else return true;
    } else std::cout << "The table is inexistent!" << std::endl;
    return false;
}

//bool MyDataBase::do_register(const char *accountNumber, const char *password)
//{
//    if (check_ID(accountNumber)) cout << "exit" << endl;
//    else cout << "inexit" << endl;
//    return true;
//}

//bool MyDataBase::do_login()
//{

//}

//bool MyDataBase::get_info_vod()
//{

//}

//bool MyDataBase::add_info_vod()
//{

//}

//bool MyDataBase::update_info_vod()
//{

//}

//bool MyDataBase::remove_info_vod()
//{

//}
