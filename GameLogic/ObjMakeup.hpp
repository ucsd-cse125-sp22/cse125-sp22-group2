#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjMakeup : public PhysicalObject
{
private:
	static unsigned int numMakeups; // initialized to 0

public:
	// Whether this makeup booth is occupied
	bool occupied;
	// The id for the bar that blocks the booth's exit
	unsigned int barID;
	unsigned int makeupID;

	ObjMakeup();
	ObjMakeup(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, bool solid);
	~ObjMakeup();
};