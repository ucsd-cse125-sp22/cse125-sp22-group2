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
	this->makeupLevel = MAKEUP_MAX;
	this->score = 0.0f;
	this->hasCrown = false;
	this->booth = -1;
	this->boothTime = 0.0f;
	this->gravity = 0.0f;
	this->momentum = 0.0f;
	this->maxSpeed = DEFAULT_MAX_SPEED;
	this->thresholdDecay = 0.0f;
	this->hasPowerup = false;
	this->powerupTime = 0.0f;
	this->boostTargetDirection = direction;

	this->activatePowerup = false;
	this->crashed = false;
	this->tookCrown = false;
	this->gotPowerup = false;
	this->bounced = false;
	this->honked = false;

	this->distribution = normal_distribution<float>(0, 1);
	// Respawning
	//this->distributionInt = uniform_int_distribution<int>(0, 3);
	//this->speedModifier = 30.0f / cse125config::TICK_RATE;
}

ObjPlayer::~ObjPlayer() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjPlayer::step(float gameTime) {
	// Reset triggers
	this->activatePowerup = false;
	this->crashed = false;
	this->tookCrown = false;
	this->gotPowerup = false;
	this->bounced = false;
	this->honked = false;

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
		makeupLevel = max(0.0f, makeupLevel - MAKEUP_DECREASE_RATE / cse125config::TICK_RATE);
	}

	// Increase score, currently set to not increase while invincible/stunning/fixing makeup
	// Eventually this will probably be based on the amount of time left in the match, which
	// would probably be passed in as a parameter
	//if (hasCrown && booth == -1 && !stun) {
	if (hasCrown && !stun) {
		const float scoreModifier = SCORE_INCREASE * SCORE_WEIGHT * (cse125config::MATCH_LENGTH - gameTime) / cse125config::MATCH_LENGTH;
		score += (SCORE_INCREASE + scoreModifier) / cse125config::TICK_RATE;
	}

	// Makeup station
	if (booth != -1 && boothTime) {
		makeupLevel = MAKEUP_MAX;
		glm::vec3 destination = (1.5f * position + objects->at(booth)->position) / 2.5f;
		glm::vec3 dir = (1.5f * direction + objects->at(booth)->direction) / 2.5f;
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
			speed = SPEED_LEAVE_BOOTH;
			boostTargetDirection = glm::normalize((2.0f * objects->at(booth)->direction + 1.5f * glm::normalize(objects->at(booth)->position) * -1.0f) / 3.5f);
			iframes = MAKEUP_IFRAMES * cse125config::TICK_RATE;
		}
		
	}
	if (booth != -1 && !objectPosition(this->boundingBox, oMakeup)) {
		((ObjMakeup*)objects->at(booth))->occupied = false;
		// Note: We are letting the makeup booth handle this itself now
		// objects->at(((ObjMakeup*)objects->at(booth))->barID)->solid = true;
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
		if (glm::dot(direction, boostTargetDirection) < 0.95f) {
			glm::vec3 targetDir = lerp(direction, boostTargetDirection, min((SPEED_THRESHOLD * 0.25f) / (pow(speed, 3.0f) * SPEED_THRESHOLD), 1.0f));
			BoundingBox bb = generateBoundingBox(position, targetDir, this->up);
			if (checkPlaceFree(bb)) {
				this->direction = targetDir;
				this->boundingBox = bb;
			}
		}
		speed -= thresholdDecay;
		thresholdDecay += 0.01f * TICK_FACTOR;
	}
	else {
		boostTargetDirection = direction;
		thresholdDecay = 0.0f;
		// Lower speed if above maxSpeed
		if (speed > maxSpeed) {
			speed = max(maxSpeed, speed - (SPEED_DECAY * TICK_FACTOR));
		}
	}

	// Update powerup time
	if (powerupTime) {
		powerupTime = max(0.0f, powerupTime - 1.0f / cse125config::TICK_RATE);
		speed = max(speed, POWERUP_SPEED);
		ObjTrail* t = new ObjTrail(this->objects, this->objects->size(), this->id, this->position - this->direction * TICK_FACTOR * this->speed / 2.0f, this->direction, this->up);
		if (createTrailCheck(t)) {
			objects->push_back(t);
		}
		else {
			delete t;
		}
	}

	// Gravity
	applyGravity();

	//if (!id) {
	//	cout << detectFloor(this->boundingBox) << "\n";
	//}
	// TODO: uncomment this probably
	//if (!id) {
	//	applyGravity();
	//	matchTerrain();
	//}
	//matchTerrain();
}

void ObjPlayer::action(glm::vec3 dir) {
	if (this->activatePowerup) {
		if (hasPowerup && !stun && !boothTime) {
			this->powerupTime = POWERUP_TIME;
			this->hasPowerup = false;
		}
	}

	// Can't move when stunned or locked in booth
	if (!stun && !boothTime) {
		// SLOPES!!!
		//glm::vec3 angledDir = dir;
		//if (this->up.y < 0.9f) {
		//	angledDir = glm::normalize(glm::cross(dir, glm::cross(this->up, dir)));
		//	if (glm::dot(angledDir, dir) < 0.0f) {
		//		angledDir = -angledDir;
		//	}
		//}
		//glm::vec3 newDir = angledDir; //glm::normalize(lerp(angledDir, this->direction, min(1.0f, speed / SPEED_THRESHOLD)));
		//cout << up.x << "    " << up.y << "    " << up.z << " up\n";
		//cout << newDir.x << "    " << newDir.y << "    " << newDir.z << " newDir\n";

		// Increase speed (Note: if we are above the max speed we need to ignore this)
		if (speed < maxSpeed) {
			speed = min(maxSpeed, speed + (SPEED_FORCE * TICK_FACTOR));
		}

		// Set direction
		glm::vec3 newDir = glm::normalize(lerp(dir, this->direction, min(1.0f, pow(speed / SPEED_THRESHOLD, TICK_FACTOR))));
		if (glm::dot(newDir, dir) > 0.9999f) {
			newDir = dir;
		}

		// Allow breaking away from walls while using a powerup
		if (powerupTime && momentum == 0.0f) {
			speed = 0.0f;
			momentum = 0.01f;
			newDir = glm::normalize(lerp(this->direction, dir, 0.15f));
		}

		// Move
		move(newDir);
	}
	else if (!boothTime) {
		// Still move due to inertia if stunned
		idle();
	}
	// cout << id << " " << hasCrown << " " << position.x << ", " << position.z << "\n";
}

void ObjPlayer::idle() {
	if (this->activatePowerup) {
		if (hasPowerup && !stun && !boothTime) {
			this->powerupTime = POWERUP_TIME;
			this->hasPowerup = false;
		}
	}

	if (speed < SPEED_THRESHOLD) {
		momentum = max(0.0f, momentum - (MOMENTUM_DECAY * TICK_FACTOR));
		speed = max(0.0f, speed - (SPEED_DECAY * TICK_FACTOR));
	}
	if (speed > 0.0f && !boothTime) {
		move(direction);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjPlayer::move(glm::vec3 dir) {
	// Where we are trying to move (might change during collision loop)
	glm::vec3 destination = this->position + (this->speed * TICK_FACTOR) * dir;

	// Generate a bounding box at our destination and check what we would collide with
	BoundingBox bb = generateBoundingBox(destination, dir, this->up);

	// Make sure we don't drive out of the arena
	glm::vec3 arenaAdjustment = bounding::checkCollisionRadius(bb, MAP_CENTER, MAP_RADIUS);
	if (glm::length(arenaAdjustment) > 0.0f) {
		if (!iframes && momentum >= MOMENTUM_CRASH_THRESHOLD) {
			this->crashed = true;
		}
		momentum = 0.0f;
		if (speed < SPEED_THRESHOLD) {
			speed = 0.0f;
		}
		destination += arenaAdjustment;
		bb = generateBoundingBox(destination, dir, this->up);
	}

	// Check for the floor
	if (this->position.y < -0.1f && detectFloor(bb)) {
		destination = this->position;
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
		bool pushCheck = false;
		// Push other players
		if (obj->type == oPlayer) {
			glm::vec3 d = glm::normalize(obj->position - this->position);
			if (glm::dot(d, dir) > 0.0f) {
				pushCheck = ((ObjPlayer*)obj)->movePushed(dir, glm::dot(d, dir * this->speed * TICK_FACTOR));
			}
			
			// Don't waste adjustment if player was pushed entirely out of the way
			//if (!bounding::checkCollision(bb, obj->boundingBox)) {
			//	// Crashed, so momentum is reset
			//	if (!iframes && momentum >= MOMENTUM_CRASH_THRESHOLD) {
			//		this->crashed = true;
			//		this->momentum = 0.0f;
			//	}
			//	continue;
			//}
		}

		// Try to enter a makeup station
		if (obj->type == oMakeup) {
			if (!((ObjMakeup*)obj)->occupied && ((ObjMakeup*)obj)->ready) {
				potentialBooth = obj->id;
			}
		}

		// A solid object is blocking us
		if (obj->solid && !adjusted) {
			destinationFree = false;

			// Crashed, so momentum is reset
			if (!iframes && momentum >= MOMENTUM_CRASH_THRESHOLD) {
				this->crashed = true;

			}
			momentum = 0.0f;
			if (speed < SPEED_THRESHOLD) {
				if (pushCheck) {
					speed = min(speed, maxSpeed * 0.5f);
				}
				else {
					speed = 0.0f;
				}
			}

			//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
			glm::vec3 adjust = bounding::checkCollisionAdjust(bb, obj->boundingBox);
			//cout <<  " Shifting " << glm::length(adjust) << " ";
			if (glm::length(adjust) < MAX_ADJUSTMENT) {
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
		// Pick up powerup
		pickupPowerup(obj);
	}

	// If our destination is free, complete the move
	if (destinationFree) {
		this->momentum = min(MAX_MOMENTUM, momentum + glm::distance(destination, position));

		this->position = destination;
		this->direction = dir;
		this->boundingBox = bb;

		//matchTerrain();

		// Enter the makeup station if there was one at our destination
		if (potentialBooth != -1) {
			PhysicalObject*& obj = this->objects->at(potentialBooth);
			if (!((ObjMakeup*)obj)->occupied && ((ObjMakeup*)obj)->ready) {
				((ObjMakeup*)obj)->occupied = true;
				((ObjMakeup*)obj)->ready = false;
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
			((ObjPlayer*)obj)->tookCrown = true;
			((ObjPlayer*)obj)->iframes = CROWN_IFRAMES * cse125config::TICK_RATE;
			this->hasCrown = false;
			this->stun = STEAL_STUN_FRAMES * cse125config::TICK_RATE;
			this->momentum = 0.0f;
		}
		// The other player has the crown, take it
		else if (((ObjPlayer*)obj)->hasCrown && !((ObjPlayer*)obj)->iframes) {
			this->hasCrown = true;
			this->tookCrown = true;
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
			this->tookCrown = true;
			this->iframes = CROWN_IFRAMES * cse125config::TICK_RATE;
			// Too easy to drive off of the edge
			this->speed = SPEED_STEAL_CROWN / 1.5f;
		}
	}
}

void ObjPlayer::pickupPowerup(const PhysicalObject* obj) {
	if (obj->type == oPowerup && !this->hasPowerup) {
		if (((ObjPowerup*)obj)->spawned) {
			((ObjPowerup*)obj)->spawned = false;
			((ObjPowerup*)obj)->respawnTime = POWERUP_RESPAWN_TIME + ((ObjPowerup*)obj)->distributionNormal(((ObjPowerup*)obj)->generator);
			this->hasPowerup = true;
			this->gotPowerup = true;
		}
	}
}

bool ObjPlayer::objectPositionTagged(BoundingBox bb, int type, unsigned int id) {
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (i == this->id) {
			continue;
		}
		if (this->objects->at(i)->type == type && type == oTrail && ((ObjTrail*)this->objects->at(i))->playerID != id) {
			if (bounding::checkCollision(bb, this->objects->at(i)->boundingBox)) {
				return true;
			}
		}
	}
	return false;
}

bool ObjPlayer::createTrailCheck(ObjTrail* newTrail) {
	int objCount = this->objects->size();
	for (unsigned int i = 0; i < objCount; i++) {
		if (this->objects->at(i)->type == oTrail && ((ObjTrail*)this->objects->at(i))->playerID == this->id && bounding::checkCollision(newTrail->boundingBox, this->objects->at(i)->boundingBox)) {
			return false;
		}
	}
	return true;
}

bool ObjPlayer::movePushed(glm::vec3 dir, float pushSpeed) {
	glm::vec3 destination = this->position + pushSpeed * dir;
	BoundingBox bb = generateBoundingBox(destination, this->direction, this->up);
	bool result = true;
	// Make sure we don't get pushed out of the arena
	glm::vec3 arenaAdjustment = bounding::checkCollisionRadius(bb, MAP_CENTER, MAP_RADIUS);
	if (glm::length(arenaAdjustment) > 0.0f) {
		if (!iframes && momentum >= MOMENTUM_CRASH_THRESHOLD) {
			this->crashed = true;
		}
		momentum = 0.0f;
		destination += arenaAdjustment;
		bb = generateBoundingBox(destination, dir, this->up);
		result = false;
	}

	// Check for the floor
	if (this->position.y < -0.1f && detectFloor(bb)) {
		destination = this->position;
	}

	vector<int> collisions = findCollisionObjects(bb);
	for (unsigned int i = 0; i < collisions.size(); i++) {
		// Push other players
		if (this->objects->at(collisions[i])->type == oPlayer) {
			glm::vec3 d = glm::normalize(this->objects->at(collisions[i])->position - this->position);
			if (glm::dot(d, dir) > 0.25f) {
				((ObjPlayer*)this->objects->at(collisions[i]))->movePushed(dir, glm::dot(d, dir * pushSpeed));
			}
		}

		if (this->objects->at(collisions[i])->solid) {
			if (!iframes && momentum >= MOMENTUM_CRASH_THRESHOLD) {
				this->crashed = true;
			}
			momentum = 0.0f;
			result = false;
			return result;
		}
	}
	this->momentum = min(MAX_MOMENTUM, momentum + glm::distance(destination, position));
	this->position = destination;
	this->boundingBox = bb;
	return result;
}

bool ObjPlayer::detectFloor(BoundingBox bb, glm::vec3 center) {
	if (bounding::checkWithinRadii(this->boundingBox, center, MAP_PIT_RADIUS_INNER, MAP_PIT_RADIUS_OUTER)) {
		if ((bb.bbMin.x > MAP_PIT_ROAD_HALF_WIDTH || bb.bbMax.x < -MAP_PIT_ROAD_HALF_WIDTH) &&
			(bb.bbMin.z > MAP_PIT_ROAD_HALF_WIDTH || bb.bbMax.z < -MAP_PIT_ROAD_HALF_WIDTH)) {
			return false;
		}
	}
	return true;
}

void ObjPlayer::applyGravity() {
	// Check logical floor
	bool f = detectFloor(this->boundingBox);

	// Check whether we are in the air
	if (this->position.y != 0.0f || !f) {
		//float offset = 0.01f;
		//if (this->gravity < 0.0f) {
		//	offset = -0.01f
		//}
		BoundingBox bb = generateBoundingBox(this->position - glm::vec3(0.0f, 0.01f, 0.0f), this->direction, this->up);
		// Check whether something is below us
		if (checkPlaceFree(bb)) {
			// Increase the amount gravity is pulling us
			this->gravity = min(this->gravity + (GRAVITY_FORCE * TICK_FACTOR), GRAVITY_MAX);
			glm::vec3 destination = this->position - glm::vec3(0.0f, this->gravity * TICK_FACTOR, 0.0f);
			// Make sure we don't fall through the floor
			if (f && this->position.y > 0.0f) {
				destination.y = max(destination.y, 0.0f);
			}

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
					// If the object is above us, cancel potential upwards velocity
					if (obj->position.y > this->position.y) {
						this->gravity = max(this->gravity, 0.0f);
					}
					//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
					glm::vec3 adjust = bounding::checkCollisionAdjust(bb, obj->boundingBox);
					//cout <<  " Shifting " << glm::length(adjust) << " ";
					if (glm::length(adjust) < MAX_ADJUSTMENT) {
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
				// Pick up powerup
				pickupPowerup(obj);
			}

			// If our destination is free, complete the move
			if (destinationFree) {
				this->position = destination;
				this->boundingBox = bb;
			}
		}
		else {
			// Check if there is a player right below us
			int tempID = findObjectPosition(bb, oPlayer);
			if (tempID != -1) {
				// Transfer/take the crown
				crownTransfer(this->objects->at(tempID));
			}

			// We have something solid beneath us, so reset gravity
			this->gravity = 0.2f;
			this->speed = max(this->speed, DEFAULT_MAX_SPEED);
			glm::vec3 newDir = this->direction + glm::normalize(glm::vec3(distribution(generator), 0.0f, distribution(generator)));
			newDir = glm::normalize(lerp(this->direction, newDir, 0.25f));
			bb = generateBoundingBox(this->position, newDir, this->up);
			if (checkPlaceFree(bb)) {
				this->direction = newDir;
			}
		}
	}
	else {
		// We are on the ground, reset gravity
		this->gravity = 0.0f;
	}

	// Let players get back onto the ground if they are only slightly below it
	if (f && position.y > -0.15f && position.y < 0.0f && gravity > 0.0f) {
		BoundingBox bb = generateBoundingBox(glm::vec3(this->position.x, 0.0f, this->position.y), this->direction, this->up);
		if (checkPlaceFree(bb)) {
			this->position.y = 0.0f;
			this->boundingBox = bb;
			this->gravity = 0.0f;
		}
	}


	// TODO REMOVE THIS AFTER SLOPES/RESPAWNING
	//if (position.y < -8.0f) {
	//	position.y = 8.0f;
	//	boundingBox = generateBoundingBox(position, this->direction, this->up);
	//	gravity = 0.0f;
	//}

	//if (this->position.y < -6.0f) {
	//	this->gravity = -0.8f;
	//	this->bounced = true;
	//}
	// Trampoline
	if (this->position.y < -4.725f) {
		this->gravity = -0.732f;
		this->bounced = true;
	}

	// Respawn
	//if (this->position.y < -6.0f) {
	//	do
	//	{
	//		int respawn = distributionInt(generator);
	//		this->position = respawnLocations[respawn];
	//		this->direction = respawnDirections[respawn];
	//		this->boundingBox = generateBoundingBox(this->position, this->direction, this->up);
	//	} while (!checkPlaceFree(boundingBox));
	//}
}

void ObjPlayer::matchTerrain() {

	//void ObjPlayer::applyGravity() {
	//	BoundingBox bb = generateBoundingBox(this->position - glm::vec3(0.0f, 0.01f, 0.0f), this->direction, this->up);
	//
	//	// Check whether we are in the air
	//	if (checkPlaceFree(bb) && position.y > -12.0f) {
	//
	//		// Increase the amount gravity is pulling us
	//		this->gravity = min(this->gravity + GRAVITY_FORCE, GRAVITY_MAX);
	//		glm::vec3 destination = this->position - glm::vec3(0.0f, this->gravity, 0.0f);
	//
	//		// Generate a bounding box at our destination and check what we would collide with
	//		bb = generateBoundingBox(destination, this->direction, this->up);
	//
	//		// Get the collisions for our destination
	//		vector<int> collisions = findCollisionObjects(bb);
	//
	//		// Use to determine whether to cancel the move
	//		bool destinationFree = true;
	//		// Attempt to move out of a single collision (this is limited to one attempt to avoid an infinite loop)
	//		bool adjusted = false;
	//
	//		// Go through every object we collided with (this includes non-solids that we can overlap with)
	//		for (unsigned int i = 0; i < collisions.size(); i++) {
	//			PhysicalObject*& obj = this->objects->at(collisions[i]);
	//
	//			// A solid object is blocking us
	//			if (obj->solid && !adjusted) {
	//				destinationFree = false;
	//				//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
	//				glm::vec3 adjust = bounding::checkCollisionAdjust(bb, obj->boundingBox);
	//				//cout <<  " Shifting " << glm::length(adjust) << " ";
	//				if (glm::length(adjust) < MAX_ADJUSTMENT) {
	//					//cout << "Adjusting?  Dot:" << glm::dot(dir, adjust) << " ";
	//					if (glm::dot(obj->position - this->position, adjust) > 0) {
	//						//cout << " FLIPPING ";
	//						adjust = -adjust;
	//					}
	//					BoundingBox temp = generateBoundingBox(destination + adjust, this->direction, this->up);
	//					if (checkPlaceFree(temp)) {
	//						//cout << "Adjusted\n";
	//						destination += adjust;
	//						bb = temp;
	//						destinationFree = true;
	//						adjusted = true;
	//					}
	//					else {
	//						//cout << "Cancelled\n";
	//						destinationFree = false;
	//					}
	//				}
	//				else {
	//					//cout << "Confirmed\n";
	//					destinationFree = false;
	//				}
	//			}
	//
	//			// Transfer/take the crown
	//			crownTransfer(obj);
	//			// Pick up powerup
	//			pickupPowerup(obj);
	//		}
	//
	//		// If our destination is free, complete the move
	//		if (destinationFree) {
	//			this->position = destination;
	//			this->boundingBox = bb;
	//		}
	//	}
	//	else {
	//		// We have something solid beneath us, so reset gravity
	//		this->gravity = 0.0f;
	//	}
	//
	//	// REMOVE THIS AFTER SLOPES/RESPAWNING
	//	if (position.y < -8.0f) {
	//		position.y = 8.0f;
	//		boundingBox = generateBoundingBox(position, this->direction, this->up);
	//	}
	//}

	////this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	BoundingBox bb = this->boundingBox;
	// For each vertex, find the floor beneath it

	// Complex
	for (unsigned int i = 0; i < 4; i++) {
		cout << (bb.vertices[i] + glm::vec3(0.0f, -0.5f, 0.0f)).x << " " << (bb.vertices[i] + glm::vec3(0.0f, -0.5f, 0.0f)).y << " " << (bb.vertices[i] + glm::vec3(0.0f, -0.5f, 0.0f)).z << "\n";
		int floor = findObjectPosition(BoundingBox(this->id, bb.vertices[i] + glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 0.1f, 1.0f), oFloor);
		if (floor == -1) {
			// No floor below this point
			cout << "/ - ";
			bb.vertices[i] += glm::vec3(0.0f, -0.25f, 0.0f);
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

	// Simple
	//glm::vec3 newUp = glm::vec3(0.0f);
	//int floor = findObjectPosition(generateBoundingBox(this->position + glm::vec3(0.0f, -1.0f, 0.0f), this->direction, this->up), oFloor);
	//if (floor == -1) {
	//	newUp = glm::vec3(0.0f, 1.0f, 0.0f);
	//}
	//else {
	//	newUp = this->objects->at(floor)->up;
	//}
	//if (glm::dot(newUp, glm::vec3(0.0f, 1.0f, 0.0f)) < 0.0f) {
	//	newUp = -newUp;
	//}
	//cout << newUp.x << " " << newUp.y << " " << newUp.z << " - up\n";

	glm::vec3 newDir = glm::normalize(glm::cross(newUp, glm::cross(this->direction, newUp)));
	if (glm::dot(newDir, this->direction) < 0.0f) {
		newDir = -newDir;
	}
	if (glm::dot(newDir, this->direction) < 0.5f) {
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	}
	cout << direction.x << " " << direction.y << " " << direction.z << " - old direction\n";
	cout << newDir.x << " " << newDir.y << " " << newDir.z << " - new direction\n";

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

			//cout << "!COLLISION!  " << " " << width << " " << height << "; ";
			glm::vec3 adjust = bounding::checkCollisionAdjust(bb, obj->boundingBox);
			//cout <<  " Shifting " << glm::length(adjust) << " ";
			if (glm::length(adjust) < MAX_ADJUSTMENT) {
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
		//cout << destinationFree << " (" << position.x << " " << position.y << " " << position.z << ")\n"
		//	<< " (" << destination.x << " " << destination.y << " " << destination.z << ")\n"
		//	<< " (" << newDir.x << " " << newDir.y << " " << newDir.z << ")\n"
		//	<< " (" << newUp.x << " " << newUp.y << " " << newUp.z << ")\n\n";
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
