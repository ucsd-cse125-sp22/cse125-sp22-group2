#pragma once

#include "core.h"
#include "BoundingBox.hpp"

using namespace bounding;

class PhysicalObject
{
private:

public:
	// This is pointer back to the vector containing all objects
	vector<PhysicalObject*>* objects;
	// This is the index of this objects in the vector of all objects
	unsigned int id;
	// Whether this object is solid (for now all objects will be solid)
	bool solid;
	// The current speed of this object
	float speed;
	// Whether this object can move, most likely unnecessary
	// bool dynamic;
	// This object's bounding box
	BoundingBox boundingBox;

	glm::vec3 position;
	float length, width, height;

	// Direction the object is facing and which direction is up
	glm::vec3 direction;
	glm::vec3 up;
	// Could store a matrix instead
	// glm::mat4 transform;

	PhysicalObject();
	PhysicalObject(vector<PhysicalObject*>* objects, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, unsigned int id, bool solid);
	~PhysicalObject();

	// Create a bounding box
	BoundingBox generateBoundingBox(glm::vec3 pos, glm::vec3 dir);
	// Basic boolean collision check that terminates immediately if a collision with a solid object is found
	bool checkPlaceFree(glm::vec3 pos, glm::vec3 dir);
	// Longer collision check that collects the id of every object collided with
	vector<int> findCollisionObjects(glm::vec3 pos, glm::vec3 dir);
	void movePosition(glm::vec3 pos);
	void moveDirection(glm::vec3 dir);
};