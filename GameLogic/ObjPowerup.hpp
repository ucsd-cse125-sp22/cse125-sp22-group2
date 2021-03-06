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
	// Randomness
	default_random_engine generator;
	uniform_int_distribution<int> distribution;
	normal_distribution<float> distributionNormal;

	ObjPowerup();
	ObjPowerup(vector<PhysicalObject*>* objects, unsigned int id, vector<glm::vec3> locations, glm::vec3 direction, glm::vec3 up);
	~ObjPowerup();

	void step();
};