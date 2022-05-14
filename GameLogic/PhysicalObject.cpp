#include "PhysicalObject.hpp"
PhysicalObject::PhysicalObject() {
	//this->objects = nullptr;
	//this->id = 0;

	//this->position = glm::vec3(0.0f);
	//this->length = 0.0f;
	//this->width = 0.0f;
	//this->height = 0.0f;

	//this->speed = 1.0f;
	//this->boundingBox = generateBoundingBox(position, direction, up);

	//this->direction = glm::vec3(0.0f);
	//this->up = glm::vec3(0.0f);

	//this->solid = false;
}

PhysicalObject::PhysicalObject(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, bool solid) {
	this->objects = objects;
	this->id = id;

	this->position = position;
	this->length = length;
	this->width = width;
	this->height = height;

	this->speed = 1.0f;
	this->boundingBox = generateBoundingBox(position, direction, up);

	this->direction = direction;
	this->up = up;

	this->solid = solid;
}

PhysicalObject::~PhysicalObject() {

}

BoundingBox PhysicalObject::generateBoundingBox(glm::vec3 pos, glm::vec3 dir, glm::vec3 up) {
	return BoundingBox(this->id, pos, dir, up, this->length, this->width, this->height);
}

bool PhysicalObject::checkPlaceFree(BoundingBox bb) {
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == id) {
			continue;
		}
		if (this->objects->at(i)->solid && bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			return false;
		}
	}
	return true;
}

vector<int> PhysicalObject::findCollisionObjects(BoundingBox bb) {
	int objCount = this->objects->size();
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

bool PhysicalObject::objectPosition(BoundingBox bb, int type) {
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == id) {
			continue;
		}
		if (this->objects->at(i)->type == type && bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			return true;
		}
	}
	return false;
}

int PhysicalObject::findObjectPosition(BoundingBox bb, int type) {
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == id) {
			continue;
		}
		if (this->objects->at(i)->type == type && bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			return i;
		}
	}
	return -1;
}

void PhysicalObject::movePosition(glm::vec3 pos) {
	BoundingBox bb = generateBoundingBox(pos, this->direction, this->up);
	vector<int> collisions = findCollisionObjects(bb);
	for (unsigned int i = 0; i < collisions.size(); i++) {
		if (this->objects->at(collisions[i])->solid) {
			return;
		}
	}
	this->position = pos;
	this->boundingBox = bb;
}

void PhysicalObject::moveDirection(glm::vec3 dir) {
	glm::vec3 destination = this->position + this->speed * dir;
	BoundingBox bb = generateBoundingBox(destination, dir, this->up);
	vector<int> collisions = findCollisionObjects(bb);
	for (unsigned int i = 0; i < collisions.size(); i++) {
		if (this->objects->at(collisions[i])->solid) {
			return;
		}
	}
	this->position = destination;
	this->direction = dir;
	this->boundingBox = bb;
}

void PhysicalObject::step() {
}