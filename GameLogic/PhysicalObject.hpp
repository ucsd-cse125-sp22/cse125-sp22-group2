#pragma once

#include "core.h"
#include "BoundingBox.hpp"
#include "Collision.hpp"

using namespace collision;

class PhysicalObject
{
private:

public:
	vector<PhysicalObject*>* objects;
	unsigned int id;
	bool solid;
	bool speed;
	// bool dynamic;
	BoundingBox boundingBox;
	glm::vec3 position;
	glm::vec3 direction;
	glm::mat4 transform;

	PhysicalObject();
	~PhysicalObject();

	BoundingBox generateBoundingBox(glm::vec3 pos, glm::vec3 dir);
	bool checkCollision(glm::vec3 pos, glm::vec3 dir);
	vector<int> findCollisionObjects(glm::vec3 pos, glm::vec3 dir);
	void movePosition(glm::vec3 pos);
	void moveDirection(glm::vec3 pos, glm::vec3 dir);
};