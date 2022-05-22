#ifndef _CORE_H_
#define _CORE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <vector>
#include <ctype.h>
#include "../Constants.hpp"
#include "../Config.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

// Game parameters
const float MATCH_LENGTH = 10.0f; //180.0f

// Map
const glm::vec3 MAP_CENTER = glm::vec3(0.0f);
const float MAP_RADIUS = 63.5f;

// Makeup
const float MAKEUP_MAX = 100.0f;
const float MAKEUP_DECREASE_RATE = 1.0f;
const float MAKEUP_BOOTH_TIME = 10.0f;

// Max Speeds
const float DEFAULT_MAX_SPEED = 0.5f;
const float CROWN_SPEED_BOOST = 0.0f;
const float MAKEUP_SPEED_PENALTY = 0.25f;
const float TRAIL_SPEED_PENALTY = 0.25f;

// Speeds
const float MOMENTUM_DECAY = 0.1f;
const float SPEED_DECAY = 0.1f;
const float THRESHOLD_DECAY_GROWTH = 0.05f;
const float SPEED_FORCE = 0.1f;
const float SPEED_THRESHOLD = 0.75f;
const float SPEED_STEAL_CROWN = 1.0f;

// Gravity
const float GRAVITY_MAX = 0.1f;
const float GRAVITY_FORCE = 0.001f;

// Invincibility Frames
const float CROWN_IFRAMES = 2.0f;
const float MAKEUP_IFRAMES = 2.0f;

// Stun Frames
const float STEAL_STUN_FRAMES = 0.25f;

// Dimensions (oPlayer)
const float PLAYER_LENGTH = 2.1f;
const float PLAYER_WIDTH = 1.15f;
const float PLAYER_HEIGHT = 0.98f;

// Dimensions (oBooth)
const float BOOTH_WALL_L_LENGTH = 5.6f;
const float BOOTH_WALL_L_WIDTH = 0.5f;
const float BOOTH_WALL_L_HEIGHT = 2.98f;
const float BOOTH_WALL_R_LENGTH = 5.6f;
const float BOOTH_WALL_R_WIDTH = 1.85f;
const float BOOTH_WALL_R_HEIGHT = 2.98f;
const float BOOTH_BAR_LENGTH = 2.1f;
const float BOOTH_BAR_WIDTH = 1.15f;
const float BOOTH_BAR_HEIGHT = 0.98f;
const float BOOTH_MAKEUP_LENGTH = 1.0f;
const float BOOTH_MAKEUP_WIDTH = 1.0f;
const float BOOTH_MAKEUP_HEIGHT = 1.0f;
const glm::vec3 BOOTH_WALL_L_OFFSET = glm::vec3(-1.92f, 0.0f, 0.157f); // glm::vec3(-3.0f, 0.0f, 6.0f);
const glm::vec3 BOOTH_WALL_R_OFFSET = glm::vec3(2.2f, 0.0f, 0.157f);
const glm::vec3 BOOTH_BAR_OFFSET = glm::vec3(0.0f, 0.0f, 0.0f);

#endif