#include "PhysicalObject.hpp"
PhysicalObject::PhysicalObject() {
	this->type = oOther;
	this->objects = nullptr;
	this->id = 0;

	this->position = glm::vec3(0.0f);
	this->length = 0.0f;
	this->width = 0.0f;
	this->height = 0.0f;

	this->speed = 1.0f;

	this->direction = glm::vec3(0.0f);
	this->up = glm::vec3(0.0f);

	//this->boundingBox = generateBoundingBox(position, direction, up);

	this->solid = false;
}

PhysicalObject::~PhysicalObject() {

}

BoundingBox PhysicalObject::generateBoundingBox(glm::vec3 pos, glm::vec3 dir, glm::vec3 up) {
	return BoundingBox(this->id, pos, dir, up, this->length, this->width, this->height);
}

bool PhysicalObject::checkPlaceFree(BoundingBox bb) {
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == this->id) {
			continue;
		}
		if (this->objects->at(i)->solid && bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			return false;
		}
	}
	return true;
}

bool PhysicalObject::checkPlaceEmpty(BoundingBox bb)
{
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == this->id) {
			continue;
		}
		if (bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			return false;
		}
	}
	return true;
}

vector<int> PhysicalObject::findCollisionObjects(BoundingBox bb) {
	int objCount = this->objects->size();
	vector<int> collisions = vector<int>();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == id || type == oTrail) {
			continue;
		}
		if (bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			collisions.push_back(i);
		}
	}
	return collisions;
}

vector<int> PhysicalObject::findCollisionObjectsType(BoundingBox bb, int type) {
	int objCount = this->objects->size();
	vector<int> collisions = vector<int>();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == id || this->objects->at(i)->type != type) {
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

float PhysicalObject::lerp(float v1, float v2, float t) {
	return v1 + (v2 - v1) * t;
}

glm::vec3 PhysicalObject::lerp(glm::vec3 v1, glm::vec3 v2, float t) {
	if (glm::dot(v1, v2) < -0.999f) {
		v1 = glm::normalize(v1 + glm::vec3(0.01f, 0.0f, 0.01f));
	}
	return v1 + (v2 - v1) * t;
}