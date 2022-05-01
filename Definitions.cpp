#include "Definitions.hpp"

std::ostream& operator<<(std::ostream& os, const vec3& v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const vec4& v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z <<", " << v.w << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const MovementKey& movementKey)
{
	std::string str;
	switch (movementKey) {
	case MovementKey::FORWARD:
		str = "FORWARD";
		break;
	case MovementKey::BACKWARD:
		str = "BACKWARD";
		break;
	case MovementKey::RIGHT:
		str = "RIGHT";
		break;
	case MovementKey::LEFT:
		str = "LEFT";
		break;
	}

	os << str;
	return os;
}
