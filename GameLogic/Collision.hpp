#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "core.h"
#include <iostream>
#include "BoundingBox.hpp"
#include "PhysicalObject.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace collision
{
	bool checkCollision(BoundingBox a, BoundingBox b);
	vector<vector<int>*>* createGrid(vector<PhysicalObject*>* objects, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
	void removeFromGrid(vector<vector<int>*>* uniformGrid, PhysicalObject* obj);
	void addToGrid(vector<vector<int>*>* uniformGrid, PhysicalObject* obj, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
}

////////////////////////////////////////////////////////////////////////////////

#endif