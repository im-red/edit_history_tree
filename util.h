#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <iostream>

namespace util
{
class Timer
{
public:
    using Clock = std::chrono::high_resolution_clock;
    Timer()
    {
        m_begin = Clock::now();
    }
    ~Timer()
    {
        endAndOutput();
    }
    void begin()
    {
        m_begin = Clock::now();
    }
    void end()
    {
        m_end = Clock::now();
    }
    void output()
    {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_begin);
        std::cout << us.count() << " us" << std::endl;
    }
    void endAndOutput()
    {
        end();
        output();
    }

private:
    Clock::time_point m_begin;
    Clock::time_point m_end;
};
}

#endif // UTIL_H
