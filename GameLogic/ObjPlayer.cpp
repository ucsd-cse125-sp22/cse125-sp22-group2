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
	this->length = PLAYER_LENGTH;
	this->width = PLAYER_WIDTH;
	this->height = PLAYER_HEIGHT;

	this->direction = direction;
	this->up = up;
	this->boundingBox = generateBoundingBox(position, direction, up);

	this->solid = true;

	// Specific to player
	this->speed = 0.0f;
	this->iframes = 0;
	this->stun = 0;
	this->makeupLevel = 100.0f;
	this->score = 0.0f;
	this->hasCrown = false;
	this->booth = -1;
	this->boothTime = 0.0f;
	this->momentum = 0.0f;
	this->thresholdDecay = 0.0f;
}

ObjPlayer::~ObjPlayer() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	if (makeupLevel && booth == -1 && !objectPosition(boundingBox, oMakeup)) {
		makeupLevel -= 1.0f / cse125config::DEFAULT_TICK_RATE;
	}

	// Increase score, currently set to not increase while invincible/stunning/fixing makeup
	// Eventually this will probably be based on the amount of time left in the match, which
	// would probably be passed in as a parameter
	if (hasCrown && booth == -1 && !stun && !iframes) {
		score += 1.0f / cse125config::DEFAULT_TICK_RATE;
	}

	// Makeup station
	if (booth != -1 && boothTime) {
		glm::vec3 destination = (position + objects->at(booth)->position) / 2.0f;
		glm::vec3 dir = (direction + objects->at(booth)->direction) / 2.0f;
		BoundingBox bb = generateBoundingBox(destination, dir, this->up);
		if (checkPlaceFree(bb)) {
			this->position = destination;
			this->direction = dir;
			this->boundingBox = bb;
		}
		boothTime -= 1.0f / cse125config::TICK_RATE;
		if (boothTime <= 0.0f) {
			// TODO: potentially change this to work with tick rate
			boothTime = 0.0f;
			objects->at(((ObjMakeup*)objects->at(booth))->barID)->solid = false;
			makeupLevel = MAKEUP_MAX;
			speed = SPEED_LEAVE_BOOTH;
			iframes = MAKEUP_IFRAMES;

		}
		
	}
	if (booth != -1 && !objectPosition(this->boundingBox, oMakeup)) {
		((ObjMakeup*)objects->at(booth))->occupied = false;
		objects->at(((ObjMakeup*)objects->at(booth))->barID)->solid = true;
		booth = -1;
	}

	// Adjust max speed (these numbers are placeholders)
	maxSpeed = DEFAULT_MAX_SPEED;
	if (hasCrown) {
		maxSpeed += CROWN_SPEED_BOOST;
	}
	if (objectPositionTagged(boundingBox, oTrail, id)) {
		maxSpeed -= TRAIL_SPEED_PENALTY;
	}
	if (makeupLevel < 0.01f) {
		maxSpeed -= MAKEUP_SPEED_PENALTY;
	}

	// Adjust speed above threshold
	if (speed > SPEED_THRESHOLD) {
		speed -= thresholdDecay;
		thresholdDecay += 0.01f;
	}
	else {
		thresholdDecay = 0.0f;
		// Lower speed if above maxSpeed
		if (speed > maxSpeed) {
			speed = max(maxSpeed, speed - SPEED_DECAY);
		}
	}

	//if (!id) {
	//	cout << momentum << " " << makeupLevel << " " << booth << " " << boothTime << "\n";
	//}

	// TODO: uncomment this probably
	if (!id) {

		applyGravity();
		matchTerrain();
	}
	//applyGravity();
	//matchTerrain();
}

void ObjPlayer::action(glm::vec3 dir) {
	// Can't move when stunned or locked in booth
	if (!stun && !boothTime) {
		// Increase speed (Note: if we are above the max speed we need to ignore this)
		if (speed < maxSpeed) {
			speed = min(maxSpeed, speed + SPEED_FORCE);
		}

		glm::vec3 newDir = glm::normalize(lerp(dir, this->direction, min(1.0f, speed / SPEED_THRESHOLD)));

		// Move
		move(newDir);
	}
	// cout << id << " " << hasCrown << " " << position.x << ", " << position.z << "\n";
}

void ObjPlayer::idle() {
	// If we above a certain threshold, the player should not be able to control their movement as well
	if (speed < SPEED_THRESHOLD) {
		momentum = max(0.0f, momentum - MOMENTUM_DECAY);
		speed = max(0.0f, speed - SPEED_DECAY);
	}
	if (speed > 0.0f) {
		move(direction);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjPlayer::move(glm::vec3 dir) {
	// Where we are trying to move (might change during collision loop)
	glm::vec3 destination = this->position + this->speed * dir;
	// Generate a bounding box at our destination and check what we would collide with
	BoundingBox bb = generateBoundingBox(destination, dir, this->up);

	// Make sure we don't drive out of the arena
	glm::vec3 arenaAdjustment = bounding::checkCollisionRadius(bb, MAP_CENTER, MAP_RADIUS);
	if (glm::length(arenaAdjustment) > 0.0f) {
		destination += arenaAdjustment;
		bb = generateBoundingBox(destination, dir, this->up);
	}

	// Get the collisions for our destination
	vector<int> collisions = findCollisionObjects(bb);

	// Use to determine whether to cancel the move
	bool destinationFree = true;
	// Attempt to move out of a single collision (this is limited to one attempt to avoid an infinite loop)
	bool adjusted = false;
	// The booth at our destination
	int potentialBooth = -1;

	// Go through every object we collided with (this includes non-solids that we can overlap with)
	for (unsigned int i = 0; i < collisions.size(); i++) {
		PhysicalObject*& obj = this->objects->at(collisions[i]);

		// Push other players
		if (obj->type == oPlayer) {
			glm::vec3 d = glm::normalize(obj->position - this->position);
			((ObjPlayer*)obj)->movePushed(dir, glm::dot(d, dir * this->speed));
		}

		// Try to enter a makeup station
		if (obj->type == oMakeup) {
			if (!((ObjMakeup*)obj)->occupied) {
				potentialBooth = obj->id;
			}
		}

		// A solid object is blocking us
		if (obj->solid && !adjusted) {
			destinationFree = false;

			// Crashed, so momentum is reset
			momentum = 0.0f;
			if (speed < SPEED_THRESHOLD) {
				speed = 0.0f;
			}

			//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
			glm::vec3 adjust = bounding::checkCollisionAdjust(bb, obj->boundingBox);
			//cout <<  " Shifting " << glm::length(adjust) << " ";
			if (glm::length(adjust) < 0.5f) {
				//cout << "Adjusting?  Dot:" << glm::dot(dir, adjust) << " ";
				if (glm::dot(obj->position - this->position, adjust) > 0) {
					//cout << " FLIPPING ";
					adjust = -adjust;
				}
				BoundingBox temp = generateBoundingBox(destination + adjust, dir, this->up);
				if (checkPlaceFree(temp)) {
					//cout << "Adjusted\n";
					destination += adjust;
					bb = temp;
					destinationFree = true;
					adjusted = true;
				}
				else {
					//cout << "Cancelled\n";
					destinationFree = false;
				}
			}
			else {
				//cout << "Confirmed\n";
				destinationFree = false;
			}
		}

		// Transfer/take the crown
		crownTransfer(obj);
	}

	// If our destination is free, complete the move
	if (destinationFree) {
		this->momentum = min(MAX_MOMENTUM, momentum + glm::distance(destination, position));

		this->position = destination;
		this->direction = dir;
		this->boundingBox = bb;

		matchTerrain();

		// Enter the makeup station if there was one at our destination
		if (potentialBooth != -1) {
			PhysicalObject*& obj = this->objects->at(potentialBooth);
			if (!((ObjMakeup*)obj)->occupied) {
				((ObjMakeup*)obj)->occupied = true;
				this->booth = obj->id;
				// Placeholder value
				this->boothTime = MAKEUP_BOOTH_TIME;
			}
		}
	}
}

void ObjPlayer::crownTransfer(const PhysicalObject* obj) {
	if (obj->type == oPlayer) {
		// We have the crown, pass it to the other player
		if (this->hasCrown && !iframes) {
			((ObjPlayer*)obj)->hasCrown = true;
			((ObjPlayer*)obj)->iframes = CROWN_IFRAMES * cse125config::TICK_RATE;
			this->hasCrown = false;
			this->stun = STEAL_STUN_FRAMES * cse125config::TICK_RATE;
			this->momentum = 0.0f;
		}
		// The other player has the crown, take it
		else if (((ObjPlayer*)obj)->hasCrown && !((ObjPlayer*)obj)->iframes) {
			this->hasCrown = true;
			this->iframes = CROWN_IFRAMES * cse125config::TICK_RATE;
			this->speed = SPEED_STEAL_CROWN;
			((ObjPlayer*)obj)->hasCrown = false;
			((ObjPlayer*)obj)->stun = STEAL_STUN_FRAMES * cse125config::TICK_RATE;
			((ObjPlayer*)obj)->momentum = 0.0f;
		}
	}
	// The crown is loose, take it
	else if (obj->type == oCrown) {
		if (((ObjCrown*)obj)->loose) {
			((ObjCrown*)obj)->loose = false;
			this->hasCrown = true;
			this->iframes = CROWN_IFRAMES * cse125config::TICK_RATE;
		}
	}
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

void ObjPlayer::movePushed(glm::vec3 dir, float pushSpeed) {
	glm::vec3 destination = this->position + pushSpeed * dir;
	BoundingBox bb = generateBoundingBox(destination, this->direction, this->up);

	// Make sure we don't get pushed out of the arena
	glm::vec3 arenaAdjustment = bounding::checkCollisionRadius(bb, MAP_CENTER, MAP_RADIUS);
	if (glm::length(arenaAdjustment) > 0.0f) {
		destination += arenaAdjustment;
		bb = generateBoundingBox(destination, dir, this->up);
	}

	vector<int> collisions = findCollisionObjects(bb);
	for (unsigned int i = 0; i < collisions.size(); i++) {
		if (this->objects->at(collisions[i])->solid) {
			return;
		}
	}
	this->position = destination;
	this->boundingBox = bb;
}

void ObjPlayer::applyGravity() {
	BoundingBox bb = generateBoundingBox(this->position - glm::vec3(0.0f, 0.01f, 0.0f), this->direction, this->up);

	// Check whether we are in the air
	if (checkPlaceFree(bb) && position.y > -12.0f) {

		// Increase the amount gravity is pulling us
		this->gravity = min(this->gravity + GRAVITY_FORCE, GRAVITY_MAX);
		glm::vec3 destination = this->position - glm::vec3(0.0f, this->gravity, 0.0f);

		// Generate a bounding box at our destination and check what we would collide with
		bb = generateBoundingBox(destination, this->direction, this->up);

		// Get the collisions for our destination
		vector<int> collisions = findCollisionObjects(bb);

		// Use to determine whether to cancel the move
		bool destinationFree = true;
		// Attempt to move out of a single collision (this is limited to one attempt to avoid an infinite loop)
		bool adjusted = false;

		// Go through every object we collided with (this includes non-solids that we can overlap with)
		for (unsigned int i = 0; i < collisions.size(); i++) {
			PhysicalObject*& obj = this->objects->at(collisions[i]);

			// A solid object is blocking us
			if (obj->solid && !adjusted) {
				destinationFree = false;
				//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
				glm::vec3 adjust = bounding::checkCollisionAdjust(bb, obj->boundingBox);
				//cout <<  " Shifting " << glm::length(adjust) << " ";
				if (glm::length(adjust) < 0.5f) {
					//cout << "Adjusting?  Dot:" << glm::dot(dir, adjust) << " ";
					if (glm::dot(obj->position - this->position, adjust) > 0) {
						//cout << " FLIPPING ";
						adjust = -adjust;
					}
					BoundingBox temp = generateBoundingBox(destination + adjust, this->direction, this->up);
					if (checkPlaceFree(temp)) {
						//cout << "Adjusted\n";
						destination += adjust;
						bb = temp;
						destinationFree = true;
						adjusted = true;
					}
					else {
						//cout << "Cancelled\n";
						destinationFree = false;
					}
				}
				else {
					//cout << "Confirmed\n";
					destinationFree = false;
				}
			}

			// Transfer/take the crown
			crownTransfer(obj);
		}

		// If our destination is free, complete the move
		if (destinationFree) {
			this->position = destination;
			this->boundingBox = bb;
		}
	}
	else {
		// We have something solid beneath us, so reset gravity
		this->gravity = 0.0f;
	}

	// REMOVE THIS
	if (position.y < -8.0f) {
		position.y = 8.0f;
		boundingBox = generateBoundingBox(position, this->direction, this->up);
	}
}

void ObjPlayer::matchTerrain() {
	//this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	BoundingBox bb = this->boundingBox;
	// For each vertex, find the floor beneath it
	for (unsigned int i = 0; i < 4; i++) {
		int floor = findObjectPosition(BoundingBox(this->id, bb.vertices[i] + glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 0.1f, 1.0f), oFloor);
		if (floor == -1) {
			// No floor below this point
			cout << "/ - ";
			//bb.vertices[i] += glm::vec3(0.0f, -0.25f, 0.0f);
		}
		else {
			// Adjust based on the floor
			glm::vec3 adj = checkCollisionPointFloor(bb.vertices[i], this->objects->at(floor)->boundingBox);
			cout << "/ (" << adj.x << " " << adj.y << " " << adj.z << ") ";
			bb.vertices[i] += adj;
		}
	}
	cout << "/\n";

	glm::vec3 newUp = glm::normalize(glm::cross(bb.vertices[0] - bb.vertices[2], bb.vertices[1] - bb.vertices[3]));
	if (glm::dot(newUp, glm::vec3(0.0f, 1.0f, 0.0f)) < 0.0f) {
		newUp = -newUp;
	}

	glm::vec3 newDir = glm::normalize(glm::cross(glm::cross(this->direction, newUp), newUp));
	if (glm::dot(newDir, this->direction) < 0.0f) {
		newDir = -newDir;
	}

	glm::vec3 destination = this->position;

	// Generate a bounding box at our destination and check what we would collide with
	bb = generateBoundingBox(destination, newDir, newUp);

	// Get the collisions for our destination
	vector<int> collisions = findCollisionObjects(bb);

	// Use to determine whether to cancel the move
	bool destinationFree = true;
	// Attempt to move out of a single collision (this is limited to one attempt to avoid an infinite loop)
	bool adjusted = false;

	// Go through every object we collided with (this includes non-solids that we can overlap with)
	for (unsigned int i = 0; i < collisions.size(); i++) {
		PhysicalObject*& obj = this->objects->at(collisions[i]);

		// A solid object is blocking us
		if (obj->solid && !adjusted) {
			destinationFree = false;

			// Crashed, so momentum is reset
			momentum = 0.0f;
			if (speed < SPEED_THRESHOLD) {
				speed = 0.0f;
			}

			//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
			glm::vec3 adjust = bounding::checkCollisionAdjust(bb, obj->boundingBox);
			//cout <<  " Shifting " << glm::length(adjust) << " ";
			if (glm::length(adjust) < 0.5f) {
				//cout << "Adjusting?  Dot:" << glm::dot(dir, adjust) << " ";
				if (glm::dot(obj->position - this->position, adjust) > 0) {
					//cout << " FLIPPING ";
					adjust = -adjust;
				}
				BoundingBox temp = generateBoundingBox(destination + adjust, newDir, newUp);
				if (checkPlaceFree(temp)) {
					//cout << "Adjusted\n";
					destination += adjust;
					bb = temp;
					destinationFree = true;
					adjusted = true;
				}
				else {
					//cout << "Cancelled\n";
					destinationFree = false;
				}
			}
			else {
				//cout << "Confirmed\n";
				destinationFree = false;
			}
		}
	}

	if (!id) {
		cout << destinationFree << " (" << position.x << " " << position.y << " " << position.z << ")\n"
			<< " (" << destination.x << " " << destination.y << " " << destination.z << ")\n"
			<< " (" << newDir.x << " " << newDir.y << " " << newDir.z << ")\n"
			<< " (" << newUp.x << " " << newUp.y << " " << newUp.z << ")\n\n";
	}

	// If our destination is free, complete the move
	if (destinationFree) {
		this->momentum = min(MAX_MOMENTUM, momentum + glm::distance(destination, position));

		this->position = destination;
		this->direction = newDir;
		this->up = newUp;
		this->boundingBox = bb;
	}



	// Since previous attempts kind of went nowhere this is now blank
	// This is my plan for slopes though
	// 
	// Part I: Offset all lower vertices to be above the floor
	// Part II: Use the diagonals to get the up vector
	// Part III: Construct a new bounding box
	// Part IV: Adjust the bounding box again to not collide with the floor
}

