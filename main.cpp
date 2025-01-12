/*
func: 主函数。
author: zouyujie
date: 2024.12.2
*/
#include "myserver.h"

int main(int argc, char* argv[])
{
    MyServer myServer;
    myServer.launch();  //循环监听
    return 0;
}
