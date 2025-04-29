/*
func: 主函数。
author: zouyujie
date: 2024.12.2
*/
#include <filesystem>  //C++17

#include "dbbroker.h"
#include "myserver.h"
#include "boostserver.h"
#include "config.h"

int main(int argc, char* argv[])
{
    /* 创建文件夹 */
    try {
        std::filesystem::create_directory(PROFILE_PICTURE_URL);
        std::cout << "目录创建成功: " << PROFILE_PICTURE_URL << std::endl;
        std::filesystem::create_directory(CHAT_PICTURE_URL);
        std::cout << "目录创建成功: " << CHAT_PICTURE_URL << std::endl;
        std::filesystem::create_directory(PREVIEW_PICTURE_URL);
        std::cout << "目录创建成功: " << PREVIEW_PICTURE_URL << std::endl;
        std::filesystem::create_directory(VIDEO_URL);
        std::cout << "目录创建成功: " << VIDEO_URL << std::endl;
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "无法创建目录: " << e.what() << std::endl;
    }
    /* 数据库初始化 */
    Singleton<DbBroker>::getInstance()->initDataBase();
    /* 启动服务器1 */
//    MyServer myServer;
//    myServer.launch();  //循环监听
    /* 启动服务器2 */
    BoostServer boostServer;
    boostServer.launch();  //循环监听
    return 0;
}
