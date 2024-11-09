#pragma once

#include <iostream>

class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    static Timestamp now();
    //表示const成员函数，表示不会修改成员变量
    std::string toString() const;
private:
    int64_t microSecondsSinceEpoch_;
};