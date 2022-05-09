#ifndef _PARTICLESOURCE_H_
#define _PARTICLESOURCE_H_

#include "Particle.h"
#include <string>
#include <map>
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <cstdlib>
#include <queue>
#include <random>
#include <GL/glew.h>

////////////////////////////////////////////////////////////////////////////////

#define MAX_PARTICLES 1000

class ParticleSource
{
private:
	glm::vec3 position;
	glm::vec3 velocity; 
	float creationRate; 
	float roundOffError;
	float lifeSpan; 
	float mass;
	float windSpeed; 
	glm::vec3 windDirection;

	float positionVariance, velocityVariance, lifespanVariance; 
	float gravity, airDensity, drag; 
	float particleRadius; 
	float collisionElasticity, collisionFriction;

	//std::vector<Particle*> particles;
	Particle* particles[MAX_PARTICLES];
	int numParticles;

	std::default_random_engine gen;
	std::normal_distribution<float> distribution{0.0f, 1.0f};

public:

	ParticleSource(glm::vec3 p, glm::vec3 v, float m, float windSp,
					glm::vec3 windDir, float createRate = 1, float lifeSp = 100,
					float posVar = 0, float velVar = 0, float lifespVar = 0, float g = 9.8,
					float airDen = 1.225, float d = 0.47, float particleRad = 0.01,
					float colElast = 0, float colFrict = 0) {
		position = p; 
		velocity = v; 
		mass = m; 
		creationRate = createRate;
		lifeSpan = lifeSp;
		numParticles = 0;
		windSpeed = windSp; 
		windDirection = windDir;
		roundOffError = 0;

		positionVariance = posVar; 
		velocityVariance = velVar;
		lifespanVariance = lifespVar; 
		gravity = g; 
		airDensity = airDen; 
		drag = d; 
		particleRadius = particleRad; 
		collisionElasticity = colElast; 
		collisionFriction = colFrict;
	}

	void Update(float deltaTime, glm::vec3 p, glm::vec3 v, float m, float windSp,
		glm::vec3 windDir, float createRate, float lifeSp,
		float posVar, float velVar, float lifespVar, float gravity,
		float airDensity, float drag, float particleRad,
		float collisionElasticity, float collisionFriction, float floor);
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);

	void CleanUp();
};

////////////////////////////////////////////////////////////////////////////////

#endif