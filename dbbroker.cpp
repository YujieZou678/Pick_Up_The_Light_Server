/*
func: 数据库经纪人的实现。
author: zouyujie
date: 2024.12.2
*/
#include "dbbroker.h"

#include <string>
using std::string;

DbBroker::DbBroker(const char* accountNumber, const char* password)
{
    if (conn.connect("Server", "localhost", accountNumber, password)) {
        std::cout << "DB Database: Successfully connect!" << std::endl;
    }
    else {
        perror("DB Database: connection failed");
        exit(1);
    }
}

DbBroker::~DbBroker()
{
    conn.disconnect();
    std::cout << std::endl;
    std::cout << "Successfully drop the connection!" << std::endl;
}

bool DbBroker::check_ID(const char *id)
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

//bool DbBroker::do_register(const char *accountNumber, const char *password)
//{
//    if (check_ID(accountNumber)) cout << "exit" << endl;
//    else cout << "inexit" << endl;
//    return true;
//}

//bool DbBroker::do_login()
//{

//}

//bool DbBroker::get_info_vod()
//{

//}

//bool DbBroker::add_info_vod()
//{

//}

//bool DbBroker::update_info_vod()
//{

//}

//bool DbBroker::remove_info_vod()
//{

//}
