/*
func: 单例模板类。
author: zouyujie
date: 2025.2.24
*/
#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton
{
private:
    Singleton() = default;
    ~Singleton() = default;

private:
    /* 既然不能构造，那么没发现禁用拷贝/移动赋值的意义 */
    Singleton(const Singleton&a) = delete;
    Singleton(const Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(const Singleton&&) = delete;

public:
    static T* getInstance()
    {
        static T instance;  //局部静态变量初始化线程安全 C++11
        return &instance;
    }
};

#endif // SINGLETON_H
