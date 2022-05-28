#ifndef _PARTICLE_H_
#define _PARTICLE_H_

//#include "dof.h"
//#include "Tokenizer.h"
#include "ParticleCube.h" 
#include <string>
#include <map>
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <cstdlib>
#include <GL/glew.h>

////////////////////////////////////////////////////////////////////////////////

class Particle
{
private:
	glm::vec3 OriginalPosition;
	glm::vec3 Position; 
	glm::vec3 Velocity; 
	glm::vec3 Force; 
	glm::vec3 normal;
	float Mass;
	float lifespan;
	float radius;

public:
	bool fixed;

	Particle(glm::vec3& p, glm::vec3& v, glm::vec3& f, float m, bool fix, float lifesp = 10000000000000000, float rad = 0.01);

	glm::vec3 getPosition() { return Position; }
	glm::vec3 getVelocity() { return Velocity; }
	glm::vec3 getForce() { return Force; }
	float getMass() { return Mass; }
	glm::vec3 getNormal() { return normal; }
	float getLifespan() { return lifespan; }
	float getRadius() { return radius; }

	void setPosition(glm::vec3 p) { Position.x = p.x; Position.y = p.y; Position.z = p.z; }
	void setForce(glm::vec3 f) { Force.x = f.x; Force.y = f.y; Force.z = f.z; }
	void setMass(float m) { Mass = m; }
	void setVelocity(glm::vec3 v) { Velocity = v; }

	void setNormal(glm::vec3 n);
	void addNormal(glm::vec3 n);
	void normalizeNormal();

	void ApplyPositionChange(float x, float y, float z);
	void ApplyForce(glm::vec3& f) { Force += f; }

	void Integrate(float deltaTime) {

		lifespan -= deltaTime;

		if (!fixed) {
			glm::vec3 accel = (1 / Mass) * Force; 
			Velocity += accel * deltaTime; 
			Position += Velocity * deltaTime; 
			Force = glm::vec3(0.0);
		}
	}

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
};

////////////////////////////////////////////////////////////////////////////////

#endif