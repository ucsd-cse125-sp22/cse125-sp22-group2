#pragma once

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <chrono>
#include <cstdlib>

namespace cse125clocktick
{

class ClockTick
{
  public:
    /**
     * @brief Construct a new Clock Tick object
     *
     * @param tickrate tick rate in Ticks per Second. range: [1,1000000]
     */
    ClockTick(uint16_t tickrate = 30);

    /**
     * @brief Beginning of tick loop. Initializes timer for iteration.
     *
     */
    void tickStart();
    /**
     * @brief End of tick loop. Sleeps to keep loop at steady rate.
     *
     */
    void tickEnd();

  private:
    uint16_t tickrate;
    boost::chrono::microseconds period;
    boost::chrono::microseconds starttick;
};

}