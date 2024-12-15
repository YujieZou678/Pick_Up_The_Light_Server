/*
func: 线程池的实现。
author: zouyujie
date: 2024.12.2
*/
#include "threadpool.h"

#include <iostream>
using std::cout;
using std::endl;

#include "config.h"

ThreadPool::ThreadPool(int min, int max)
{
    this->min_num = min;
    this->max_num = max;
    this->live_num = min;
    this->busy_num = 0;
    this->destroy_num = 0;

    this->shutdown = false;

    /* 初始化管理者线程 */
    this->manager_thread = thread(manager_execute, this);
    /* 初始化工作者线程 */
    this->worker_threads.resize(max_num);
    for (int i=0; i<min_num; i++) {
        this->worker_threads[i] = thread(worker_execute, this);
    }
}

void ThreadPool::add_task(Task &task)
{
    unique_lock<mutex> lk(this->poll_mutex);  //加锁
    if (this->shutdown) return;
    taskQ.push(task);
    this->poll_cond.notify_all();  //唤醒线程执行
}

ThreadPool::~ThreadPool()
{
    unique_lock<mutex> lk(this->poll_mutex);  //加锁，此处不加锁可能出现各种问题，如：相互阻塞，异常终止等。
    this->shutdown = true;
    this->destroy_num = 0;
    lk.unlock();  //必须解锁，否则相互阻塞
    /* 阻塞回收管理者线程 */
    if (this->manager_thread.joinable()) this->manager_thread.join();
    /* 唤醒并阻塞回收工作者线程 */
    this->poll_cond.notify_all();
    for (int i=0; i<worker_threads.size(); i++) {
        if (worker_threads[i].joinable()) worker_threads[i].join();
    }
}

int ThreadPool::get_live_num()
{
    unique_lock<mutex> lk(this->poll_mutex);  //加锁
    return this->live_num;
}

int ThreadPool::get_busy_num()
{
    unique_lock<mutex> lk(this->poll_mutex);  //加锁
    return this->busy_num;
}

void ThreadPool::detach_cur_thread(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);  //得到当前线程池对象

    for (int i=0; i<pool->worker_threads.size(); i++) {
        if (pool->worker_threads[i].get_id() == std::this_thread::get_id()) {
            if (pool->worker_threads[i].joinable()) pool->worker_threads[i].detach();
            break;
        }
    }
}

void ThreadPool::worker_execute(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);  //得到当前线程池对象

    while (1) {
        unique_lock<mutex> lk(pool->poll_mutex);  //加锁
        /* 线程池任务队列为空，且状态正常 */
        while (pool->taskQ.empty() && !pool->shutdown) {
            pool->poll_cond.wait(lk);  //沉睡当前线程

            /* 该线程被唤醒后 */
            if (pool->destroy_num > 0) {  //如果线程池需要销毁线程
                pool->destroy_num--;
                if (pool->live_num > pool->min_num) {  //满足条件则自杀
                    pool->live_num--;
                    std::cout << "threadID：" << std::this_thread::get_id() << "exit..." << std::endl;
                    /* 销毁当前线程 */
                    pool->detach_cur_thread(pool);
                    return;
                }
            }
        }
        /* 线程池任务队列不为空 */
        if (pool->shutdown) {  //如果线程池状态销毁，则自杀
            std::cout << "threadID：" << std::this_thread::get_id() << "exit..." << std::endl;
            return;
        }
        //取一个任务
        Task task = pool->taskQ.front();
        pool->taskQ.pop();
        pool->busy_num++;
        lk.unlock();  //手动解锁
        //执行该任务
        std::cout << "threadID：" << std::this_thread::get_id() << "start working..." << std::endl;
        task();
        //任务执行完毕
        std::cout << "threadID：" << std::this_thread::get_id() << "end working..." << std::endl;
        lk.lock();
        pool->busy_num--;
        lk.unlock();
    }
}

void ThreadPool::manager_execute(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);  //得到当前线程池对象

    /* 监控 */
    while (true) {
        /* 每3秒检测一次 */
        std::this_thread::sleep_for(std::chrono::seconds(3));

        unique_lock<mutex> lk(pool->poll_mutex);  //加锁

        if (pool->shutdown) {  //线程池对象销毁则自杀
            std::cout << "threadID：" << std::this_thread::get_id() << "exit..." << std::endl;
            return;
        }

        /* 添加线程的情况
         * 任务个数 > 存活线程数 && 存活线程数 < 最大线程数 */
        if (pool->taskQ.size() > pool->live_num && pool->live_num < pool->max_num) {
            int count = 0;  //添加线程个数
            for (int i=0; i<pool->max_num && count<CHANGE_THREAD_NUMBER && pool->live_num<pool->max_num; i++) {
                if (pool->worker_threads[i].get_id() == thread::id()) {
                    std::cout << "Create a new thread..." << std::endl;
                    pool->worker_threads[i] = thread(worker_execute, pool);
                    count++;
                    pool->live_num++;
                }
            }
        }
        /* 销毁线程的情况
         * 忙碌线程数*2 < 存活线程数 && 存活线程数 > 最小线程数 */
        if (pool->busy_num*2 < pool->live_num && pool->live_num > pool->min_num) {
            pool->destroy_num = CHANGE_THREAD_NUMBER;
            pool->poll_cond.notify_all();
        }
    }
}



