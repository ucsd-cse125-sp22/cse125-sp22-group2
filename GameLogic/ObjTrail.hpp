#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjTrail : public PhysicalObject
{
private:

public:
	// How much time this trail has left before disappearing
	float life;
	// The player that spawned the trail
	unsigned int playerID;

	ObjTrail();
	ObjTrail(vector<PhysicalObject*>* objects, unsigned int id, unsigned int playerID, glm::vec3 position, glm::vec3 direction, glm::vec3 up);
	~ObjTrail();

	void step();
};