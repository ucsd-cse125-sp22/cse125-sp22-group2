#include "ObjMakeup.hpp"
ObjMakeup::ObjMakeup() {
	this->objects = nullptr;
	this->id = 0;

	this->position = glm::vec3(0.0f);
	this->length = 0.0f;
	this->width = 0.0f;
	this->height = 0.0f;

	this->speed = 1.0f;
	this->boundingBox = generateBoundingBox(position, direction);

	this->direction = glm::vec3(0.0f);
	this->up = glm::vec3(0.0f);

	this->solid = false;
}

ObjMakeup::ObjMakeup(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up, bool solid) {
	this->objects = objects;
	this->id = id;

	this->position = position;
	this->length = length;
	this->width = width;
	this->height = height;

	this->speed = 1.0f;
	this->boundingBox = generateBoundingBox(position, direction);

	this->direction = direction;
	this->up = up;

	this->solid = solid;
}

ObjMakeup::~ObjMakeup() {

}