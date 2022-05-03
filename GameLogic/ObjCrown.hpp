#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjCrown : public PhysicalObject
{
private:

public:
	int type = oCrown;

	// Whether the crown is not bound to a player
	bool loose;

	ObjCrown();
	ObjCrown(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, glm::vec3 direction, glm::vec3 up);
	~ObjCrown();
};