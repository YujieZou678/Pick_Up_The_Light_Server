/*
func: 主函数。
author: zouyujie
date: 2024.12.2
*/
#include <filesystem>  //C++17

#include "dbbroker.h"
#include "myserver.h"
#include "config.h"

int main(int argc, char* argv[])
{
    /* 创建文件夹 */
    try {
        string dirname = PROFILE_PICTURE_URL;
        std::filesystem::create_directory(dirname);
        std::cout << "目录创建成功: " << dirname << std::endl;
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "无法创建目录: " << e.what() << std::endl;
    }
    /* 数据库初始化 */
    DbBroker::getInstance()->initDataBase();
    /* 启动服务器 */
    MyServer myServer;
    myServer.launch();  //循环监听
    return 0;
}
