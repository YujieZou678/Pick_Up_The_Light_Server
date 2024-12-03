#include <iostream>

#include "threadpool.h"

/* 功能：打印数据
 * 参数：count 2, int a, int b */
void print(int count, ...) {
    /* 获取参数 */
    count = 1;  //调用时所有参数用一个vector对象封装
    va_list datas;
    va_start(datas, count);
    vector<void*> parameters = va_arg(datas, vector<void*>);
    va_end(datas);
    int* a = static_cast<int*>(parameters[0]);
    int* b = static_cast<int*>(parameters[1]);

    /* 执行操作 */
    std::cout << *a << *b << std::endl;
}

int main()
{
    Task task;
    int a = 11;
    int b = 22;
    task.A(print, 2, &a, &b);
    task.B();

    return 0;
}
