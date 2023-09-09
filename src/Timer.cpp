//
//  Timer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "Timer.h"

#include <iostream>

void Timer::Start()
{
    m_start = std::chrono::high_resolution_clock::now();
}

uint64_t Timer::Elapsed(Timer::Unit unit/* = Timer::Unit::MS */) const
{
    const auto elapsed = std::chrono::high_resolution_clock::now() - m_start;

    switch(unit)
    {
        case Timer::Unit::S:
            return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        case Timer::Unit::US:
            return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        case Timer::Unit::NS:
            return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
        default:
            return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }
}

void Timer::Print(const std::string& desc, Timer::Unit unit/* = Timer::Unit::MS */) const
{
    std::cout << "[" << desc << "]: " << Elapsed(unit);

    switch(unit)
    {
        case Timer::Unit::S:
            std::cout << " s";
            break;
        case Timer::Unit::US:
            std::cout << " us";
            break;
        case Timer::Unit::NS:
            std::cout << " ns";
            break;
        default:
            std::cout << " ms";
            break;
    }

    std::cout << std::endl;
}

