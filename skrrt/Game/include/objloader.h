#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

class objloader {

public: 
static bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals, 
	std::vector<unsigned int> & out_indices
);

bool loadAssImp(
	const char * path, 
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
);

};

#endif