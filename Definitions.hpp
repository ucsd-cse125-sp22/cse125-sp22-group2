#pragma once
// Contains global definitions, like type aliases, for all files
#include <iostream>
#include <glm/glm.hpp>
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using RealNumber = double; // can change "double" to "float" if needed; allows for flexibility with floating-point values

std::ostream& operator<<(std::ostream& os, const vec3& v);
std::ostream& operator<<(std::ostream& os, const vec4& v);

enum class MovementKey {
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
	SPACE,
};

enum class GameAction {
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_FORWARD_RIGHT,
	MOVE_FORWARD_LEFT,
	MOVE_BACKWARD_LEFT,
	MOVE_BACKWARD_RIGHT,
	ACTION,
	IDLE,
};

std::ostream& operator<<(std::ostream& os, const MovementKey& movementKey);



