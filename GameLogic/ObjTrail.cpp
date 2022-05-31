#include "ObjTrail.hpp"
ObjTrail::ObjTrail() {
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

ObjTrail::ObjTrail(vector<PhysicalObject*>* objects, unsigned int id, unsigned int playerID, glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
	this->type = oTrail;

	this->objects = objects;
	this->id = id;

	this->position = position;
	this->length = TRAIL_DIMENSIONS;
	this->width = TRAIL_DIMENSIONS;
	this->height = TRAIL_DIMENSIONS;

	this->speed = 0.0f;

	this->direction = direction;
	this->up = up;
	this->boundingBox = generateBoundingBox(position, direction, up);

	this->solid = false;

	this->playerID = playerID;
	this->life = TRAIL_LIFE;
}

ObjTrail::~ObjTrail() {
}

void ObjTrail::step() {
	// Update life
	life -= 1.0f / cse125config::TICK_RATE;
}
