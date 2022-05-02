#include "ObjPlayer.hpp"
ObjPlayer::ObjPlayer() {
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

ObjPlayer::ObjPlayer(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
	// General object properties
	this->objects = objects;
	this->id = id;

	this->position = position;
	this->length = 2.0f;
	this->width = 2.0f;
	this->height = 2.0f;

	this->direction = direction;
	this->up = up;
	this->boundingBox = generateBoundingBox(position, direction);

	this->solid = true;

	// Specific to player
	this->speed = 1.0f;
	this->iframes = 0;
	this->stun = 0;
	this->makeupLevel = 0.0f;
	this->score = 0.0f;
	this->hasCrown = false;
	this->booth = false;
}

ObjPlayer::~ObjPlayer() {

}

void ObjPlayer::step() {
	if (iframes) {
		iframes--;
	}
	if (stun) {
		stun--;
	}
	if (makeupLevel && !booth && !objectPosition(boundingBox, Makeup)) {
		makeupLevel -= 1.0f / 60.0f;
	}
	if (hasCrown && !booth && !stun && !iframes) {
		score += 1.0f / 60.0f;
	}
}

void ObjPlayer::moveSelf(glm::vec3 dir) {
	if (!stun) {
		glm::vec3 destination = this->position + this->speed * dir;
		vector<int> collisions = findCollisionObjects(generateBoundingBox(destination, dir));

		bool free = true;

		for (unsigned int i = 0; i < collisions.size(); i++) {
			PhysicalObject*& obj = this->objects->at(collisions[i]);

			if (obj->solid) {
				free = false;
			}

			// Check if crown is transferred
			if (obj->type == Player) {
				if (this->hasCrown && !iframes) {
					((ObjPlayer*)obj)->hasCrown = true;
					((ObjPlayer*)obj)->iframes = 60.0f;
					this->hasCrown = false;
					this->stun = 30.0f;
				}
				else if (((ObjPlayer*)obj)->hasCrown && !((ObjPlayer*)obj)->iframes) {
					this->hasCrown = true;
					this->iframes = 60.0f;
					((ObjPlayer*)obj)->hasCrown = false;
					((ObjPlayer*)obj)->stun = 30.0f;
				}
			}
		}

		if (free) {
			this->position = destination;
			this->direction = dir;
		}
	}
}
