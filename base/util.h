#pragma once

#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include <thread>
#include <random>

namespace  BASE 
{

template <typename T>
inline std::string ToString(const T& value)
{
    std::stringstream ss;

    ss<<value;

    return ss.str();
}

template <typename T>
inline T FromString(const std::string& str)
{
    T t;
    std::stringstream ss(str);
    ss>>t;

    return t;
}


inline void SleepMilliseconds(uint64_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


inline unsigned int Random(const unsigned int& left, const unsigned int& right)
{
    static std::mt19937 rng((unsigned int)std::time(nullptr));
    std::uniform_int_distribution<unsigned int> num(left, right);
    return num(rng);
}


}