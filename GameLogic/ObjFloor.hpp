#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

using namespace bounding;

class ObjFloor : public PhysicalObject
{
private:

public:

	ObjFloor();
	ObjFloor(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, bool solid);
	~ObjFloor();
};