#pragma once
#include <string>
namespace cse125config {
	extern std::string SERVER_HOST;
	extern std::string SERVER_PORT;
	extern int NUM_PLAYERS;
	extern int TICK_RATE;

	/*
	 * @brief Initializes all of the variables in the configuration namespace using the
	 * given configuration file path
	 * 
	 * @param configPath the path to the configuration file
	 */
	void initializeConfig(std::string configPath);
}
