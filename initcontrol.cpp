#include "initcontrol.h"

#include <mysql++/mysql++.h>
#include <iostream>
using std::cerr;
using std::endl;
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "dbbroker.h"

InitControl::InitControl()
{
}

InitControl *InitControl::getInstance()
{
    static InitControl instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

bool InitControl::do_register(const string &strMsg)
{
    json jsonMsg = json::parse(strMsg);
    string id = jsonMsg["id"];
    string pw = jsonMsg["pw"];

    /* 检测账号是否重复 */
    string command = "select id from User where id = " + id;
    mysqlpp::StoreQueryResult res = DbBroker::getInstance()->query_store(command);
    if (res != NULL) {
        if (res.begin() == res.end()) {
            /* 不重复，存入数据库 */
            command = "insert into User(id, pw) values('"+id+"','"+pw+"')";
            if (DbBroker::getInstance()->query_execute(command)) return true;
        }
    }
    else cerr << "query.store() failed!" << endl;

    return false;
}

bool InitControl::do_login(const string &jsonMsg)
{
    return false;
}

