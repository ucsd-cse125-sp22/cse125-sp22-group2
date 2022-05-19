#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjWall : public PhysicalObject
{
private:

public:
	ObjWall();
	ObjWall(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up);
	~ObjWall();
};