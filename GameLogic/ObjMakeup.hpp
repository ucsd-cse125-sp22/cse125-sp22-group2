#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjMakeup : public PhysicalObject
{
private:

public:
	// Whether this makeup booth is occupied
	bool occupied;
	// The id for the bar that blocks the booth's exit
	unsigned int barID;
	//
	unsigned int makeupID;

	ObjMakeup();
	ObjMakeup(vector<PhysicalObject*>* objects, unsigned int id, unsigned int makeupID, unsigned int barID, glm::vec3 position, glm::vec3 direction);
	~ObjMakeup();
};
