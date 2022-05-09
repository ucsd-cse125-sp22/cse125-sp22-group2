#ifndef _PARTICLECUBE_H_
#define _PARTICLECUBE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <GL/glew.h>

////////////////////////////////////////////////////////////////////////////////

class ParticleCube
{
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::vec3 color;

	// Cube Information
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

public:
	ParticleCube(glm::vec3 cubeMin=glm::vec3(-1,-1,-1), glm::vec3 cubeMax=glm::vec3(1, 1, 1));
	~ParticleCube();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update();
	void setColor(glm::vec3 c) { color = c; }

	void spin(float deg);
};

////////////////////////////////////////////////////////////////////////////////

#endif