/*
func: 线程池类，定义了线程池的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
using std::vector;

/* 单个任务类
 * 存储了一个任意方法函数，及其可变参数，最大3个参数 */
class Task {
public:
    Task(void(*callback)(void*a,void*b,void*c),
         void*a=nullptr,void*b=nullptr,void*c=nullptr)
    {
        function = callback;
        parameters.push_back(a);
        parameters.push_back(b);
        parameters.push_back(c);
    }

    void execute_task() {
        function(parameters[0], parameters[1], parameters[2]);
    }

public:
    void(*function)(void*,void*,void*);  //函数指针变量
    vector<void*> parameters;
};

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
};

#endif // THREADPOOL_H
