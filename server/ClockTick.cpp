#include "ClockTick.hpp"
#include <thread>

using namespace cse125clocktick;

#define US_TO_S 1000000

ClockTick::ClockTick(uint16_t tickrate) : tickrate(tickrate)
{
    this->period = std::chrono::microseconds(US_TO_S / tickrate);
}

void ClockTick::tickStart()
{
    // set time point to current clock time
    this->starttick = std::chrono::time_point_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now())
                          .time_since_epoch();
}

void ClockTick::tickEnd()
{
    // get end time point
    auto endtick = std::chrono::time_point_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now())
                       .time_since_epoch();

    // calculate time difference
    std::chrono::microseconds delay =
        this->period - (endtick - this->starttick);
    // delay
    std::this_thread::sleep_for(delay);
}