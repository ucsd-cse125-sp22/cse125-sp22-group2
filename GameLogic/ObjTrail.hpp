#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjTrail : public PhysicalObject
{
private:

public:
	int type = Trail;

	// How many frames this trail has left before disappearing
	int life;

	ObjTrail();
	ObjTrail(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, bool solid);
	~ObjTrail();
};