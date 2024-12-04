#include <iostream>

#include "threadpool.h"


/* 功能：xxx
 * 参数：int* a, int* b */
void print(void*a,void*b,void*c) {
    /* 获取参数 */
    int* a1 = static_cast<int*>(a);
    int* b1 = static_cast<int*>(b);
    /* 执行任务 */
    std::cout << *a1 << *b1 << std::endl;
}

int main()
{
    int a = 11;
    int b = 22;
    Task task(print, &a, &b);
    task.execute_task();

    return 0;
}
