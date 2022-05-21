#pragma once
#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1 
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_FINE 4 
#define LOG_LEVEL_FINER 5
#define LOG_LEVEL_FINEST 6
 
#define DEBUG_LEVEL LOG_LEVEL_INFO

// How to use debug level: 
// if(DEBUG_LEVEL >= LOG_LEVEL_<whatever>) {
//     std::cout << "bla\n";
// }
// Prints if DEBUG_LEVEL is at least <whatever>


// This is for utilizing the top down view of the game 
#define TOP_DOWN_VIEW false
