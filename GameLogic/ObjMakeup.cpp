#include "ObjMakeup.hpp"
ObjMakeup::ObjMakeup() {
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

ObjMakeup::ObjMakeup(vector<PhysicalObject*>* objects, unsigned int id, unsigned int makeupID, unsigned int barID, glm::vec3 position, glm::vec3 direction) {
	this->type = oMakeup;

	this->objects = objects;
	this->id = id;

	this->barID = barID;

	this->position = position;
	this->length = BOOTH_MAKEUP_LENGTH;
	this->width = BOOTH_MAKEUP_WIDTH;
	this->height = BOOTH_MAKEUP_HEIGHT;

	this->direction = direction;
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);

	this->speed = 0.0f;
	this->boundingBox = generateBoundingBox(position, direction, this->up);

	this->solid = false;
	this->makeupID = makeupID;
	this->ready = true;
}

ObjMakeup::~ObjMakeup() {

}

void ObjMakeup::step() {
	// Attept to reset the bar
	if (!this->occupied && !this->ready) {
		if (!objectPosition(this->objects->at(this->barID)->boundingBox, oPlayer)) {
			this->ready = true;
			this->objects->at(this->barID)->solid = true;
		}
	}
}
