#include "ClockTick.hpp"
#include <iostream>
#include <thread>

using namespace cse125clocktick;
using namespace boost::chrono;

#define US_TO_S 1000000
//int count = 0;
//microseconds total_time = microseconds(0);

ClockTick::ClockTick(uint16_t tickrate) : tickrate(tickrate)
{
    this->period = microseconds(US_TO_S / tickrate);
}

ClockTick::~ClockTick()
{
    // std::cerr << "average sleep time: " << total_time.count() / count << "\n";
}

void ClockTick::tickStart()
{
    // set start of clock tick
    this->start = time_point_cast<microseconds>(high_resolution_clock::now());
    //count = 0;
    //total_time = microseconds(0);
}

void ClockTick::tickEnd()
{
    // delay one period after previous deadline
    this->start += this->period;
    //auto now = time_point_cast<microseconds>(high_resolution_clock::now());
    //std::cerr << (this->start - now) << "\n";
    //total_time += (this->start - now);
    //count++;
    boost::this_thread::sleep_until(this->start);
}