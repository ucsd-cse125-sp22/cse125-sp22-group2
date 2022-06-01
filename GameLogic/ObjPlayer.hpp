#pragma once

#include "core.h"
#include "PhysicalObject.hpp"
#include "ObjCrown.hpp"
#include "ObjTrail.hpp"
#include "ObjMakeup.hpp"
#include "ObjPowerup.hpp"

class ObjPlayer : public PhysicalObject
{
private:

public:
	// The current speed of this object
	//float speed;
	// Invincibility frames
	int iframes;
	// Stun frame
	int stun;
	// Current makeup level
	float makeupLevel;
	// Total score
	float score;
	// Whether we have the crown
	bool hasCrown;
	// The makeup booth we are in, or -1 if we aren't in any
	int booth;
	// How much longer we are stuck in the booth
	float boothTime;
	// Current gravity force (represented as a positive float)
	float gravity;

	// How much we have moved without stopping or crashing
	float momentum;
	// How fast we can go
	float maxSpeed;
	// Speed drop when above the speed threshold
	float thresholdDecay;
	// Whether we have a powerup
	bool hasPowerup;
	// How much longer we have the powerup
	float powerupTime;
	// Direction to change to during boost
	glm::vec3 boostTargetDirection;

	// Triggers
	// Whether we are trying to use a powerup
	bool activatePowerup;
	// Whether we crashed this frame
	bool crashed;
	// Whether we took the crown this frame
	bool tookCrown;
	// Whether we got a powerup this frame
	bool gotPowerup;
	// Whether we bounced this frame
	bool bounced;
	// Whether we honked the horn this frame
	bool honked;

	// Change the speed depending on the tick rate
	//float speedModifier;

	// Randomness
	default_random_engine generator;
	normal_distribution<float> distribution;

	// Respawning
	//uniform_int_distribution<int> distributionInt;
	//glm::vec3 respawnLocations[4] = { glm::vec3(21.0f, 0.0f, 0.0f), glm::vec3(-21.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 21.0f), glm::vec3(0.0f, 0.0f, -21.0f) };
	//glm::vec3 respawnDirections[4] = { glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f) };


	ObjPlayer();
	ObjPlayer(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, glm::vec3 direction, glm::vec3 up);
	~ObjPlayer();

	void step(float gameTime);
	// Anything that happens because of user input goes here, calls move function to handle crown transfer
	void action(glm::vec3 dir);
	// Call when no action is performed
	void idle();

	// Movement
	void move(glm::vec3 dir);
	// Attempt crown transfer
	void crownTransfer(const PhysicalObject* obj);
	// Attempt to pickup powerup
	void pickupPowerup(const PhysicalObject* obj);
	// Special check to avoid getting objects that were made a specific player (probably only ever themselves)
	bool objectPositionTagged(BoundingBox bb, int type, unsigned int id);
	// Special movement function for when another player is pushing you
	bool movePushed(glm::vec3 dir, float pushSpeed);
	// Check whether we are in the floor region of the arena (regardless of whether we are above or below it)
	bool detectFloor(BoundingBox bb, glm::vec3 center = MAP_CENTER);
	// Apply gravity if there is no object directly below us
	void applyGravity();
	// Adhere to the terrain
	void matchTerrain();
};