#pragma once

class noncopyable {
public:
    //传入参数const noncopyable&，表示对目标对象的只读引用，同时 = delete 表示禁用拷贝构造函数
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};