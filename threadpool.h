/*
func: 线程池类，定义了线程池的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
using std::vector;
#include <stdarg.h>

/* 单个任务类
 * 存储了一个任意方法函数，及其可变参数 */
class Task {
public:
    void A(void (*callback)(int count, ...), int count, ...) {
        this->function = callback;
        this->count = count;

        /* 获取参数 */
        va_list datas;
        va_start(datas, count);
        for (int i=0; i<count; i++) {
            parameters.push_back(va_arg(datas, void*));
        }
        va_end(datas);
    }

    void B() {
        function(count, parameters);
    }

public:
    void(*function)(int count, ...);  //函数指针变量
    int count;
    vector<void*> parameters;
};

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
};

#endif // THREADPOOL_H
