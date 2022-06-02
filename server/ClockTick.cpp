#include "ClockTick.hpp"
#include <iostream>
#include <thread>

using namespace cse125clocktick;

#define US_TO_S 1000000

ClockTick::ClockTick(uint16_t tickrate) : tickrate(tickrate)
{
    this->period = boost::chrono::microseconds(US_TO_S / tickrate);
}

void ClockTick::tickStart()
{
    // set time point to current clock time
    // auto currtime = std::chrono::time_point_cast<std::chrono::microseconds>(
    //                       std::chrono::high_resolution_clock::now())
    //                       .time_since_epoch();
    auto currtime = boost::chrono::time_point_cast<boost::chrono::microseconds>(
                        boost::chrono::high_resolution_clock::now())
                        .time_since_epoch();

    auto error = currtime - this->starttick - this->period;
    std::cerr << "ClockTick error: " << error.count() << std::endl;
}

void ClockTick::tickEnd()
{
    // get end time point
    // auto endtick = std::chrono::time_point_cast<std::chrono::microseconds>(
    //                    std::chrono::high_resolution_clock::now())
    //                    .time_since_epoch();
    auto endtick = boost::chrono::time_point_cast<boost::chrono::microseconds>(
                       boost::chrono::high_resolution_clock::now())
                       .time_since_epoch();

    // calculate time difference
    boost::chrono::microseconds delay =
        this->period - (endtick - this->starttick);
    // delay
    // std::this_thread::sleep_for(delay);
    boost::this_thread::sleep_for(delay);
}