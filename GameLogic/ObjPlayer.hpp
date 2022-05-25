#pragma once

#include "core.h"
#include "PhysicalObject.hpp"
#include "ObjCrown.hpp"
#include "ObjTrail.hpp"
#include "ObjMakeup.hpp"

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
	// Whether we crashed this frame
	bool crashed;

	// How much we have moved without stopping or crashing
	float momentum;
	// How fast we can go
	float maxSpeed;
	// Speed drop when above the speed threshold
	float thresholdDecay;


	ObjPlayer();
	ObjPlayer(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, glm::vec3 direction, glm::vec3 up);
	~ObjPlayer();

	void step();
	// Anything that happens because of user input goes here, calls move function to handle crown transfer
	void action(glm::vec3 dir);
	// Call when no action is performed
	void idle();

	// Movement
	void move(glm::vec3 dir);
	// Attempt crown transfer
	void crownTransfer(const PhysicalObject* obj);
	// Special check to avoid getting objects that were made a specific player (probably only ever themselves)
	bool objectPositionTagged(BoundingBox bb, int type, unsigned int id);
	// Special movement function for when another player is pushing you
	void movePushed(glm::vec3 dir, float pushSpeed);
	// Apply gravity if there is no object directly below us
	void applyGravity();
	// Adhere to the terrain
	void matchTerrain();
};