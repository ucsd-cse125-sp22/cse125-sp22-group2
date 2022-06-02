#include "ClockTick.hpp"
#include <iostream>
#include <thread>

using namespace cse125clocktick;
using namespace boost::chrono;

#define US_TO_S 1000000

ClockTick::ClockTick(uint16_t tickrate) : tickrate(tickrate)
{
    this->period = microseconds(US_TO_S / tickrate);
}

ClockTick::~ClockTick()
{

}

void ClockTick::tickStart()
{
    // set start of clock tick
    this->start = time_point_cast<microseconds>(high_resolution_clock::now());
}

void ClockTick::tickEnd()
{
    // delay one period after previous deadline
    this->start += this->period;
    boost::this_thread::sleep_until(this->start);
}