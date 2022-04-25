#include "PhysicalObject.hpp"
PhysicalObject::PhysicalObject() {
	this->objects = nullptr;
	this->id = 0;

	this->position = glm::vec3(0.0f);
	this->length = 0.0f;
	this->width = 0.0f;
	this->height = 0.0f;

	this->speed = 0.0f;
	this->boundingBox = generateBoundingBox(position, direction);

	this->direction = glm::vec3(0.0f);
	this->up = glm::vec3(0.0f);

	this->solid = false;
}

PhysicalObject::PhysicalObject(vector<PhysicalObject*>* objects, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, unsigned int id, bool solid) {
	this->objects = objects;
	this->id = id;

	this->position = position;
	this->length = length;
	this->width = width;
	this->height = height;

	this->speed = 0.0f;
	this->boundingBox = generateBoundingBox(position, direction);

	this->direction = direction;
	this->up = up;

	this->solid = solid;
}

PhysicalObject::~PhysicalObject() {

}

BoundingBox PhysicalObject::generateBoundingBox(glm::vec3 pos, glm::vec3 dir)
{
	return BoundingBox(this->id, pos, dir, this->up, this->length, this->width, this->height);
}

bool PhysicalObject::checkPlaceFree(glm::vec3 pos, glm::vec3 dir) {
	int objCount = this->objects->size();
	BoundingBox bb = generateBoundingBox(pos, dir);
	vector<int> collisions = vector<int>();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == id) {
			continue;
		}
		if (this->objects->at(i)->solid && bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			return true;
		}
	}
	return false;
}

vector<int> PhysicalObject::findCollisionObjects(glm::vec3 pos, glm::vec3 dir)
{
	int objCount = this->objects->size();
	BoundingBox bb = generateBoundingBox(pos, dir);
	vector<int> collisions = vector<int>();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == id) {
			continue;
		}
		if (bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			collisions.push_back(i);
		}
	}
	return collisions;
}

void PhysicalObject::movePosition(glm::vec3 pos) {
	vector<int> collisions = findCollisionObjects(pos, glm::vec3(0.0f));
	for (unsigned int i = 0; i < collisions.size(); i++) {
		if (this->objects->at(collisions[i])->solid) {
			return;
		}
	}
	this->position = pos;
}

void PhysicalObject::moveDirection(glm::vec3 dir) {
	glm::vec3 destination = this->position + this->speed * dir;
	vector<int> collisions = findCollisionObjects(destination, dir);
	for (unsigned int i = 0; i < collisions.size(); i++) {
		if (this->objects->at(collisions[i])->solid) {
			return;
		}
	}
	this->position = destination;
	this->direction = dir;
}