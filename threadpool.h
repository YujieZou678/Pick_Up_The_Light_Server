/*
func: 线程池类，声明了线程池的操作。
author: zouyujie
date: 2024.12.2
*/
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
using std::vector;
#include <queue>
using std::queue;
#include <thread>
using std::thread;
#include <mutex>
using std::mutex;
#include <condition_variable>
using std::condition_variable;

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
    ~Task() {
//        for (auto parameter : parameters) {
//            if (parameter != nullptr) {
//                delete parameter;
//                parameter = nullptr;
//            }
//        }
    }

    void execute_task() {
        function(parameters[0], parameters[1], parameters[2]);
    }

public:
    void(*function)(void*,void*,void*);  //函数指针变量
    vector<void*> parameters;  //如果传入堆对象，需要手动释放，这里只传入栈对象
};

/* 线程池类 */
class ThreadPool
{
public:
    ThreadPool(int min, int max);  //最小线程数，最大线程数
    void add_task(Task);  //添加任务
    int get_live_num();  //获取当前存活线程数
    int get_busy_num();  //获取当前忙碌线程数
    ~ThreadPool();

private:
    queue<Task> taskQ;      //任务队列
    thread manager_thread;  //管理者线程
    vector<thread> worker_threads;  //工作者线程
    int min_num;   //最小线程数
    int max_num;   //最大线程数
    int live_num;  //当前存活线程数
    int busy_num;  //当前忙碌线程数
    int destroy_num;  //当前需要销毁线程数

    mutex poll_mutex;  //线程池的锁
    condition_variable poll_cond;  //线程池的条件变量
    bool shutdown;  //是否销毁线程池，默认是false
    static void manager_execute(void*);  //管理者执行任务，传递当前对象指针
    static void worker_execute(void*);   //工作者执行任务，传递当前对象指针
    static void detach_cur_thread(void*);  //分离销毁当前线程
};

#endif // THREADPOOL_H
