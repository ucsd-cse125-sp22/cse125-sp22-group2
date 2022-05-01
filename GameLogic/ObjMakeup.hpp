#pragma once

#include "core.h"
#include "PhysicalObject.hpp"

class ObjMakeup : public PhysicalObject
{
private:

public:
	int type = Makeup;

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

	// Whether this makeup booth is occupied
	bool occupied;
	// The id for the bar that blocks the booth's exit
	unsigned int barID;

	// Direction the object is facing and which direction is up
	glm::vec3 direction;
	glm::vec3 up;
	// Could store a matrix instead
	// glm::mat4 transform;

	ObjMakeup();
	ObjMakeup(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, bool solid);
	~ObjMakeup();
};