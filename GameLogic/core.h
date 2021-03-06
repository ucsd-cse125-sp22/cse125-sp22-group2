#ifndef _CORE_H_
#define _CORE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <vector>
#include <random>
#include <chrono>
#include <ctype.h>
#include "../Constants.hpp"
#include "../Config.hpp"
#include "../Frame.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace cse125config;

// Game parameters
// const float MATCH_LENGTH = 120.0f;
const float SCORE_INCREASE = 3.0f;
const float SCORE_WEIGHT = 1.0f;
const float TICK_FACTOR = 0.5f;

// Map
const glm::vec3 MAP_CENTER = glm::vec3(0.0f);
const float MAP_RADIUS = 31.5f; //63.5f; // For big map
const float MAP_PIT_RADIUS_INNER = 8.79f;
const float MAP_PIT_RADIUS_OUTER = 19.0f;
const float MAP_PIT_ROAD_HALF_WIDTH = 3.5f;

// Makeup
const float MAKEUP_MAX = 100.0f;
const float MAKEUP_DECREASE_RATE = 3.0f;
const float MAKEUP_BOOTH_TIME = 2.5f;

// Max Speeds
const float DEFAULT_MAX_SPEED = 0.5f;
const float CROWN_SPEED_BOOST = 0.05f;
const float MAKEUP_SPEED_PENALTY = 0.1f;
const float TRAIL_SPEED_PENALTY = 0.25f;
const float MAX_MOMENTUM = 15.0f;

// Speeds
const float MOMENTUM_DECAY = 1.0f;
const float MOMENTUM_CRASH_THRESHOLD = 5.0f;
const float SPEED_DECAY = 0.1f;
const float THRESHOLD_DECAY_GROWTH = 0.05f;
const float SPEED_FORCE = 0.1f;
const float SPEED_THRESHOLD = 0.75f;
const float SPEED_STEAL_CROWN = 1.2f;
const float SPEED_LEAVE_BOOTH = 1.5f;
const float MAX_ADJUSTMENT = 0.5f;

// Powerup
const float POWERUP_SPEED = 0.7f;
const float POWERUP_TIME = 3.0f;
const float POWERUP_RESPAWN_TIME = 10.0f;
const float POWERUP_DIMENSIONS = 1.0f;

// Trail
const float TRAIL_LIFE = 4.0f;
const float TRAIL_DIMENSIONS = 1.0f;

// Gravity
const float GRAVITY_MAX = 0.5f;
const float GRAVITY_FORCE = 0.0298f;

// Invincibility Frames
const float CROWN_IFRAMES = 1.5f;
const float MAKEUP_IFRAMES = 1.5f;

// Stun Frames
const float STEAL_STUN_FRAMES = 0.5f;

// Dimensions (oPlayer)
const float PLAYER_LENGTH = 2.1f;
const float PLAYER_WIDTH = 1.15f;
const float PLAYER_HEIGHT = 0.98f;

// Dimensions (oBooth)
const float BOOTH_WALL_L_LENGTH = 5.6f;
const float BOOTH_WALL_L_WIDTH = 0.9f;
const float BOOTH_WALL_L_HEIGHT = 6.0f;
//const float BOOTH_WALL_L_HEIGHT = 2.98f;
const float BOOTH_WALL_R_LENGTH = 5.6f;
const float BOOTH_WALL_R_WIDTH = 1.7f;
const float BOOTH_WALL_R_HEIGHT = 6.0f;
//const float BOOTH_WALL_R_HEIGHT = 2.98f;
const float BOOTH_BAR_LENGTH = 0.2f;
const float BOOTH_BAR_WIDTH = 4.15f;
const float BOOTH_BAR_HEIGHT = 6.0f;
//const float BOOTH_BAR_HEIGHT = 0.98f;
const float BOOTH_MAKEUP_LENGTH = 0.8f;
const float BOOTH_MAKEUP_WIDTH = 0.8f;
const float BOOTH_MAKEUP_HEIGHT = 0.8f;
const glm::vec3 BOOTH_WALL_L_OFFSET = glm::vec3(-2.12f, 0.0f, 0.157f);
const glm::vec3 BOOTH_WALL_R_OFFSET = glm::vec3(2.12f, 0.0f, 0.157f);
const glm::vec3 BOOTH_BAR_OFFSET = glm::vec3(0.0f, 0.0f, 2.0f);

#endif
