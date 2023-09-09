//
//  Timer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <cstdint>

#include <chrono>
#include <string>

class Timer
{
public:
    enum class Unit { S, MS, US, NS };

    void Start();
    uint64_t Elapsed(Timer::Unit unit = Timer::Unit::MS) const;
    void Print(const std::string& desc, Timer::Unit unit = Timer::Unit::MS) const;

private:
    std::chrono::high_resolution_clock::time_point m_start;
};

