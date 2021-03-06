#pragma once

#include "core.h"
#include "BoundingBox.hpp"

using namespace bounding;

// Object types
// To add an object type, add it to the enum and make a class that inherits PhysicalObject (format is Obj + name of object)
// Make sure its type in the .hpp is correct and add a case to createObject in PhysicalObjectManager for it
enum ObjectType { oPlayer, oWall, oFloor, oCrown, oMakeup, oTrail, oPowerup, oOther };

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
	// The current speed of this object - might be moved to only player since that is currently the only object that moves
	float speed;
	// This object's bounding box
	BoundingBox boundingBox;
	int type;

	glm::vec3 position;
	float length, width, height;

	// Direction the object is facing and which direction is up
	glm::vec3 direction;
	glm::vec3 up;
	// Could store a matrix instead
	// glm::mat4 transform;

	PhysicalObject();
	~PhysicalObject();

	// Create a bounding box
	BoundingBox generateBoundingBox(glm::vec3 pos, glm::vec3 dir, glm::vec3 up);

	// Basic boolean collision check that terminates immediately if a collision with a solid object is found
	bool checkPlaceFree(BoundingBox bb);
	// Basic boolean collision check that terminates immediately if a collision with any object is found
	bool checkPlaceEmpty(BoundingBox bb);
	// Longer collision check that collects the id of every object collided with
	vector<int> findCollisionObjects(BoundingBox bb);
	// Longer collision check that collects the id of every object collided with of the given type
	vector<int> findCollisionObjectsType(BoundingBox bb, int type);
	// Boolean collision check that terminates once a collision with the specified object is found
	bool objectPosition(BoundingBox bb, int type);
	// Collision check that terminates once a collision with the specified object is found and returns its id (-1 if not found)
	int findObjectPosition(BoundingBox bb, int type);

	// Move to a specified position if there is nothing blocking it
	void movePosition(glm::vec3 pos);
	// Move in the specified direction by this object's speed
	void moveDirection(glm::vec3 dir);
	// Update everything that needs to happen in a time step not influenced by input
	void step();

	// Linear interpolation
	float lerp(float v1, float v2, float t);
	// Linear interpolation for vec3
	glm::vec3 lerp(glm::vec3 v1, glm::vec3 v2, float t);
};