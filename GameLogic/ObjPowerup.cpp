#include "ObjPowerup.hpp"
ObjPowerup::ObjPowerup() {
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

ObjPowerup::ObjPowerup(vector<PhysicalObject*>* objects, unsigned int id, vector<glm::vec3> locations, glm::vec3 direction, glm::vec3 up) {
	this->type = oPowerup;

	this->objects = objects;
	this->id = id;

	this->distribution = uniform_int_distribution<int>(0, locations.size() - 1);
	this->distributionNormal = normal_distribution<float>(1, 1);
	generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	this->position = locations.at(distribution(generator));
	this->length = POWERUP_DIMENSIONS;
	this->width = POWERUP_DIMENSIONS;
	this->height = POWERUP_DIMENSIONS;

	this->speed = 0.0f;

	this->direction = direction;
	this->up = up;
	this->boundingBox = generateBoundingBox(position, direction, up);

	this->solid = false;

	this->spawned = false;
	this->respawnTime = POWERUP_RESPAWN_TIME / 2.0f + distributionNormal(generator);
	this->locations = locations;
}

ObjPowerup::~ObjPowerup() {

}

void ObjPowerup::step() {
	// Update respawn time
	if (respawnTime) {
		respawnTime = max(0.0f, respawnTime - 1.0f / cse125config::TICK_RATE);
	}

	if (respawnTime == 0.0f && !spawned) {
		this->position = this->locations.at(distribution(generator));
		BoundingBox bb = generateBoundingBox(this->position, this->direction, this->up);
		if (checkPlaceEmpty(bb)) {
			this->boundingBox = bb;
			this->spawned = true;
		}
	}
}