#pragma once
#include <string>
namespace cse125config {
	const std::string DEFAULT_SERVER_HOST = "localhost";
	const std::string DEFAULT_SERVER_PORT = "8000";
	const unsigned int DEFAULT_TICK_RATE = 30;
	extern std::string SERVER_HOST;
	extern std::string SERVER_PORT;
	extern int TICK_RATE;

	/*
	 * @brief Initializes all of the variables in the configuration namespace using the
	 * given configuration file path
	 * 
	 * @param configPath the path to the configuration file
	 */
	void initializeConfig(std::string configPath);
}
