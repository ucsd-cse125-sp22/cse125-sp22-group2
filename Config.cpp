#include "Config.hpp"
#include <fstream>
#include "json.hpp"
namespace cse125config {
	std::string SERVER_HOST = "defaultHost";
	std::string SERVER_PORT = "defaultPort";
	int NUM_PLAYERS = 1;
	int TICK_RATE = 30;
}

void cse125config::initializeConfig(std::string configPath) {
	// Read from the configuration file
	std::ifstream configFile(configPath);
    nlohmann::json obj;
    configFile >> obj;
	// Assign global variables
	cse125config::SERVER_HOST = obj["host"];
	cse125config::SERVER_PORT = obj["port"];
	cse125config::NUM_PLAYERS = obj["numPlayers"];    
	cse125config::TICK_RATE = obj["tickRate"];
}