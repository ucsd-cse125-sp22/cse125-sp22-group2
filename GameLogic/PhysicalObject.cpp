#include "PhysicalObject.hpp"

PhysicalObject::PhysicalObject() {
	boundingBox = BoundingBox(0, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
}

PhysicalObject::~PhysicalObject() {

}

BoundingBox PhysicalObject::generateBoundingBox(glm::vec3 pos, glm::vec3 dir)
{
	return BoundingBox(this->id, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
}

bool PhysicalObject::checkCollision(glm::vec3 pos, glm::vec3 dir) {
	
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
		if (collision::checkCollision(bb, this->objects->at(i)->boundingBox)) {
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
	position = pos;
}

void PhysicalObject::moveDirection(glm::vec3 pos, glm::vec3 dir) {
	vector<int> collisions = findCollisionObjects(pos, dir);
	for (unsigned int i = 0; i < collisions.size(); i++) {
		if (this->objects->at(collisions[i])->solid) {
			return;
		}
	}
	position = pos;
	direction = dir;
}