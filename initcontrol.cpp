#include "initcontrol.h"

#include <mysql++/mysql++.h>
#include <iostream>
using std::cerr;
using std::endl;
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "dbbroker.h"
#include "userstatusevaluator.h"

InitControl::InitControl()
{
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

bool InitControl::do_login(int fd, const string &strMsg)
{
    json jsonMsg = json::parse(strMsg);
    string id = jsonMsg["id"];
    string pw = jsonMsg["pw"];

    /* 检测密码是否正确 */
    string command = "select pw from User where id = " + id;
    mysqlpp::StoreQueryResult res = DbBroker::getInstance()->query_store(command);
    if (res != NULL) {
        if (res.begin() != res.end()) {
            auto it = res.begin();
            mysqlpp::Row row = *it;
            string real_pw = string(row[0]);
            if (pw == real_pw) {
                /* 密码正确，登陆成功 */
                Singleton<UserStatusEvaluator>::getInstance()->add_userId(fd, id);
                return true;
            }
        }
    }
    else cerr << "query.store() failed!" << endl;

    return false;
}

