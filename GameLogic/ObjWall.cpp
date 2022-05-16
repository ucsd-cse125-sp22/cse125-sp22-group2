#include "ObjWall.hpp"
ObjWall::ObjWall() {
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

ObjWall::ObjWall(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, float length, float width, float height, glm::vec3 direction, glm::vec3 up) {
	this->type = oWall;

	this->objects = objects;
	this->id = id;

	this->position = position;
	this->direction = direction;
	this->up = up;

	this->length = length;
	this->width = width;
	this->height = height;

	this->speed = 1.0f;
	this->boundingBox = generateBoundingBox(position, direction, up);

	this->solid = true;
}

ObjWall::~ObjWall() {

}