#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjPowerup : public PhysicalObject
{
private:

public:
	// Whether the powerup is spawned
	bool spawned;
	// How long until the powerup respawns
	float respawnTime;
	// List of potential spawn locations
	vector<glm::vec3> locations;
	// 
	default_random_engine generator;
	uniform_int_distribution<int> distribution = uniform_int_distribution<int>(0, cse125constants::NUM_POWERUP_LOCATIONS - 1);

	ObjPowerup();
	ObjPowerup(vector<PhysicalObject*>* objects, unsigned int id, vector<glm::vec3> locations, glm::vec3 direction, glm::vec3 up);
	~ObjPowerup();

	void step();
};