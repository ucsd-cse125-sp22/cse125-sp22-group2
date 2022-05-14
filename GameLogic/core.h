#ifndef _CORE_H_
#define _CORE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <ctype.h>
#include <algorithm>
#include "../Constants.hpp"

using namespace std;

// Game parameters
const float MATCH_LENGTH;

// Makeup
const float MAKEUP_MAX;
const float MAKEUP_DECREASE_RATE;
const float MAKEUP_BOOTH_TIME;

// Speeds
const float DEFAULT_SPEED;
const float CROWN_SPEED;
const float MAKEUP_DEPLETED_SPEED;
const float TRAIL_SPEED;

// Dimensions (oPlayer)
const float PLAYER_LENGTH;
const float PLAYER_WIDTH;
const float PLAYER_HEIGHT;

#endif