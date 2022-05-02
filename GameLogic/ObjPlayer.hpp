#pragma once

#include "core.h"
#include "PhysicalObject.hpp"
#include "ObjCrown.hpp"

class ObjPlayer : public PhysicalObject
{
private:

public:
	int type = Player;

	// The current speed of this object
	float speed;
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
	// Whether we are in a makeup booth
	bool booth;

	ObjPlayer();
	ObjPlayer(vector<PhysicalObject*>* objects, unsigned int id, glm::vec3 position, glm::vec3 direction, glm::vec3 up);
	~ObjPlayer();

	void step();
	// Movement and crown transfer, anything that happens because of user input goes here
	void action(glm::vec3 dir);
};