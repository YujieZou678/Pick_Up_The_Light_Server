/*
func: 继承该类即拥有”禁止拷贝/移动赋值“的属性。
author: zouyujie
date: 2025.2.24
*/
#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class Noncopyable
{
protected:
    Noncopyable() = default;
    ~Noncopyable() = default;

    Noncopyable(const Noncopyable&) = delete;
    Noncopyable(const Noncopyable&&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&&) = delete;
};

#endif // NONCOPYABLE_H
