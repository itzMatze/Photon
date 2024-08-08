#pragma once

#include <chrono>

template<class Precision = float>
class Timer
{
public:
    Timer() : t(std::chrono::high_resolution_clock::now())
    {}

    // default timer unit is seconds
    template<class Period = std::ratio<1, 1>>
    inline Precision restart()
    {
        Precision elapsed_time = elapsed<Period>();
        t = std::chrono::high_resolution_clock::now();
        return elapsed_time;
    }

    template<class Period = std::ratio<1, 1>>
    inline Precision elapsed() const
    {
        return std::chrono::duration<Precision, Period>(std::chrono::high_resolution_clock::now() - t).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> t;
};

