#pragma once
#include <Camera.h>

const bool ENABLE_SHADOW_MAP = true;
const unsigned int  SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;
const bool DEBUG_QUAD_VIEW = false;

const int MAX_CASCADE_LEVEL_SIZE = 16;

//const std::vector<float> SHADOW_CASCADE_LEVELS{(100.0f / CAMERA_FAR_PLANE) + CAMERA_NEAR_PLANE, (50.0f / CAMERA_FAR_PLANE) + CAMERA_NEAR_PLANE, (25.0f / CAMERA_FAR_PLANE) + CAMERA_NEAR_PLANE, (10.0f / CAMERA_FAR_PLANE) + CAMERA_NEAR_PLANE, (2.0f / CAMERA_FAR_PLANE) + CAMERA_NEAR_PLANE};
const std::vector<float> SHADOW_CASCADE_LEVELS{(50.0f / CAMERA_FAR_PLANE), (25.0f / CAMERA_FAR_PLANE), (10.0f / CAMERA_FAR_PLANE), (2.0f / CAMERA_FAR_PLANE)};

const float Z_MULT = 1.0f; // How much stuff in front of or behind the light effects the shadow
