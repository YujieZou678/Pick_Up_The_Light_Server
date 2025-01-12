/*
func: 线程池，声明了线程池的操作。
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
using std::unique_lock;
#include <condition_variable>
using std::condition_variable;
#include <functional>

using Task = std::function<void()>;  //函数对象类型

/* 线程池类 */
class ThreadPool
{
public:
    explicit ThreadPool(int min, int max);  //最小线程数，最大线程数
    template<class F,class... Args>
    void add_task(F &&f, Args&&... args) {  //完美转发(万能引用)
        Task task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        unique_lock<mutex> lk(this->poll_mutex);  //加锁
        if (this->shutdown) return;
        taskQ.push(task);
        this->poll_cond.notify_all();  //唤醒线程执行
    }

    void add_task(Task&); //重载
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
