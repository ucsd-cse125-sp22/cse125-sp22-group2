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
