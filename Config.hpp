#pragma once
#include <string>
namespace cse125config {
	const std::string DEFAULT_SERVER_HOST = "localhost";
	const std::string DEFAULT_SERVER_PORT = "8000";
	const unsigned int DEFAULT_TICK_RATE = 30;
	const float DEFAULT_MATCH_LENGTH = 30.0f;
	const bool DEFAULT_ENABLE_COUNTDOWN = false;
	const float DEFAULT_COUNTDOWN_LENGTH = 3.0f;

	extern std::string SERVER_HOST;
	extern std::string SERVER_PORT;
	extern int TICK_RATE;
	extern float MATCH_LENGTH;
	extern bool ENABLE_COUNTDOWN;
	extern float COUNTDOWN_LENGTH;

	/*
	 * @brief Initializes all of the variables in the configuration namespace using the
	 * given configuration file path
	 * 
	 * @param configPath the path to the configuration file
	 */
	void initializeConfig(std::string configPath);
}
