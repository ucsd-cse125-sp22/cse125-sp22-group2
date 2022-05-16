#ifndef _CORE_H_
#define _CORE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <ctype.h>
#include <algorithm>
#include "../Constants.hpp"
#include "../Config.hpp"

using namespace std;

// Game parameters
const float MATCH_LENGTH = 10.0f; //180.0f

// Map
const glm::vec3 MAP_CENTER = glm::vec3(0.0f);
const float MAP_RADIUS = 30.0f;

// Makeup
const float MAKEUP_MAX = 100.0f;
const float MAKEUP_DECREASE_RATE = 1.0f;
const float MAKEUP_BOOTH_TIME = 10.0f;

// Speeds
const float DEFAULT_SPEED = 0.5f;
const float CROWN_SPEED_BOOST = 0.0f;
const float MAKEUP_SPEED_PENALTY = 0.25f;
const float TRAIL_SPEED_PENALTY = 0.25f;

// Gravity
const float GRAVITY_MAX = 0.1f;
const float GRAVITY_FORCE = 0.001f;

// Invincibility Frames
const int CROWN_IFRAMES = 1;
const int MAKEUP_IFRAMES = 1;

// Stun Frames
const int STEAL_STUN = 1;

// Dimensions (oPlayer)
const float PLAYER_LENGTH = 2.1f;
const float PLAYER_WIDTH = 1.15f;
const float PLAYER_HEIGHT = 0.98f;

#endif