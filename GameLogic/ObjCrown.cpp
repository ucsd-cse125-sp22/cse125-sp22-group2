#include "ObjCrown.hpp"
ObjCrown::ObjCrown() {
	this->objects = nullptr;
	this->id = 0;

	this->position = glm::vec3(0.0f);
	this->length = 0.0f;
	this->width = 0.0f;
	this->height = 0.0f;

	this->speed = 1.0f;
	this->boundingBox = generateBoundingBox(position, direction, this->up);

	this->direction = glm::vec3(0.0f);
	this->up = glm::vec3(0.0f);

	this->solid = false;
}

ObjCrown::ObjCrown(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
	this->objects = objects;
	this->id = id;

	this->position = position;
	this->length = 1.0f;
	this->width = 1.0f;
	this->height = 1.0f;

	this->speed = 1.0f;

	this->direction = direction;
	this->up = up;
	this->boundingBox = generateBoundingBox(position, direction, up);

	this->solid = false;

	this->loose = true;
}

ObjCrown::~ObjCrown() {

}