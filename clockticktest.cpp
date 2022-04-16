#include "ClockTick.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#define TICKRATE 15 // 15 ticks per second

int main(int argc, char** argv)
{
    // use argv[1] to store clockrate
    int tickrate = argc > 1 ? atoi(argv[1]) : TICKRATE;

    // initialize ClockTick object
    cse125clocktick::ClockTick tick(tickrate);

    while (true)
    {
        // Set tick at start of loop
        tick.tickStart();

        /*** Do stuff ***/
        std::time_t t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::cout << std::endl << ctime(&t);

        std::this_thread::sleep_for(
            std::chrono::microseconds(rand() % (1000000 / tickrate - 1000)));
        /*** Do stuff ***/

        // use this to mark end of loop and delay required amount
        tick.tickEnd();
    }
}

// old to test how chrono works
int _main(int argc, char** argv)
{
    int tickrate = argc > 1 ? atoi(argv[1]) : TICKRATE;

    std::chrono::microseconds period(1000000 / tickrate);

    while (true)
    {
        std::time_t t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::cout << std::endl << ctime(&t);

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(
                         std::chrono::high_resolution_clock::now())
                         .time_since_epoch();

        /* do stuff */
        std::this_thread::sleep_for(std::chrono::microseconds(rand() % 900));

        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now())
                       .time_since_epoch();

        std::chrono::microseconds diff2 = end - start;
        std::chrono::microseconds diff3 = period - (end - start);

        std::cout << diff2.count() << " µs\n";
        std::cout << diff3.count() << " µs\n";

        std::this_thread::sleep_for(diff3);
    }

    return 0;
}