#include <fstream>
#include<iostream>
#include "Config.hpp"
#include "json.hpp"
namespace cse125config {
	std::string SERVER_HOST = cse125config::DEFAULT_SERVER_HOST;
	std::string SERVER_PORT = cse125config::DEFAULT_SERVER_PORT;
	int TICK_RATE = cse125config::DEFAULT_TICK_RATE;
}

void cse125config::initializeConfig(std::string configPath) {
	// Read from the configuration file
	std::ifstream configFile(configPath);
    nlohmann::json obj;
    configFile >> obj;
	// Assign global variables
	cse125config::SERVER_HOST = obj.value("host", DEFAULT_SERVER_HOST);
	cse125config::SERVER_PORT = obj.value("port", DEFAULT_SERVER_PORT);	
	cse125config::TICK_RATE = obj.value("tickRate", DEFAULT_TICK_RATE);	
}