#include "ObjPlayer.hpp"
ObjPlayer::ObjPlayer() {
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

ObjPlayer::ObjPlayer(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
	this->type = oPlayer;

	// General object properties
	this->objects = objects;
	this->id = id;

	this->position = position;
	// These values are the actual dimensions, might make constants for these
	this->length = 2.1f;
	this->width = 1.15f;
	this->height = 0.98f;

	this->direction = direction;
	this->up = up;
	this->boundingBox = generateBoundingBox(position, direction, up);

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
	// Update iframes
	if (iframes) {
		iframes--;
	}
	// Update stun frames
	if (stun) {
		stun--;
	}
	// Reduce makeup level if not currently fixing makeup
	if (makeupLevel && !booth && !objectPosition(boundingBox, oMakeup)) {
		makeupLevel -= 1.0f / 60.0f;
	}
	// Increase score, currently set to not increase while invincible/stunning/fixing makeup
	// Eventually this will probably be based on the amount of time left in the match, which
	// would probably be passed in as a parameter
	if (hasCrown && !booth && !stun && !iframes) {
		score += 1.0f / 60.0f;
	}
	// Adjust speed (these numbers are placeholders)
	speed = 0.5f;
	if (hasCrown) {
		//speed += 0.5f;
	}
	if (objectPositionTagged(boundingBox, oTrail, id)) {
		//speed -= 1.0f;
	}
	if (makeupLevel < 0.01f) {
		//speed -= 0.75f;
	}
}

void ObjPlayer::action(glm::vec3 dir) {
	// Can't move when stunned
	if (!stun) {
		glm::vec3 destination = this->position + this->speed * dir;
		BoundingBox bb = generateBoundingBox(destination, dir, this->up);
		vector<int> collisions = findCollisionObjects(bb);

		bool free = true;
		bool adjusted = false;

		for (unsigned int i = 0; i < collisions.size(); i++) {
			PhysicalObject*& obj = this->objects->at(collisions[i]);

			// A solid object is blocking us
			if (obj->solid && !adjusted) {
				free = false;
				//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
				glm::vec3 adjust = checkCollisionAdjust(bb, obj->boundingBox);
				//cout <<  " Shifting " << glm::length(adjust) << " ";
				if (glm::length(adjust) < 0.5f) {
					//cout << "Adjusting?  Dot:" << glm::dot(dir, adjust) << " ";
					if (glm::dot(dir, adjust) > 0) {
						//cout << " FLIPPING ";
						adjust = -adjust;
					}
					BoundingBox temp = generateBoundingBox(destination + adjust, dir, this->up);
					if (checkPlaceFree(temp)) {
						//cout << "Adjusted\n";
						destination += adjust;
						bb = temp;
						free = true;
						adjusted = true;
					}
					else {
						//cout << "Cancelled\n";
						free = false;
					}
				}
				else {
					//cout << "Confirmed\n";
					free = false;
				}
			}

			// Check if crown is transferred
			if (obj->type == oPlayer) {
				if (this->hasCrown && !iframes) {
					((ObjPlayer*)obj)->hasCrown = true;
					((ObjPlayer*)obj)->iframes = 60;
					this->hasCrown = false;
					this->stun = 30;
				}
				else if (((ObjPlayer*)obj)->hasCrown && !((ObjPlayer*)obj)->iframes) {
					this->hasCrown = true;
					this->iframes = 60;
					((ObjPlayer*)obj)->hasCrown = false;
					((ObjPlayer*)obj)->stun = 30;
				}
			}
			else if (obj->type == oCrown) {
				if (((ObjCrown*)obj)->loose) {
					((ObjCrown*)obj)->loose = false;
					this->hasCrown = true;
					this->iframes = 30;
				}
			}
		}

		// If we didn't collide, move
		if (free) {
			this->position = destination;
			this->direction = dir;
			this->boundingBox = bb;
		}
	}
	//cout << id << " " << hasCrown << " " << position.x << ", " << position.z << "\n";
}

bool ObjPlayer::objectPositionTagged(BoundingBox bb, int type, unsigned int id) {
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == this->id) {
			continue;
		}
		if (this->objects->at(i)->type == type && bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
			if (type == oTrail && ((ObjTrail*)this->objects->at(i))->playerID != id) {
				return true;
			}
		}
	}
	return false;
}
