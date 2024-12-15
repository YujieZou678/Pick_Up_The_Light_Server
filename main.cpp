/*
func: 主函数。
author: zouyujie
date: 2024.12.2
*/
#include "myserver.h"

int main(int argc, char* argv[])
{
    MyServer myServer("127.0.0.1", "2222");
    myServer.launch();  //循环监听
    return 0;
}
