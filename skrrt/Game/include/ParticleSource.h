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
#include "PartShader.h"

////////////////////////////////////////////////////////////////////////////////

#define MAX_PARTICLES 1000


//std::default_random_engine gen;
//std::uniform_real_distribution<float> distribution = std::uniform_real_distribution<float>( 0.0f, 1.0f );

/*
namespace this_sucks {
	extern std::default_random_engine gen;
	extern std::uniform_real_distribution<float> distribution;
}

*/
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
	Particle* particles[MAX_PARTICLES] = { nullptr };
	int numParticles;

	std::default_random_engine gen;

public:
	std::vector<glm::vec3> colors;
	ParticleSource(float createRate = 1, float lifeSp = 100,
					float posVar = 0, float velVar = 0, float lifespVar = 0, float g = 9.8,
					float airDen = 1.225, float d = 0.47, float particleRad = 0.1,
					float colElast = 0, float colFrict = 0, std::vector<glm::vec3> colorsInit = std::vector<glm::vec3>{ glm::vec3(1.0f) } ) {
		position = glm::vec3(0.0f, 1.0f, 0.0f); 
		velocity = glm::vec3(0.0f, 0.0f, 1.0f);
		mass = 2.0f; 
		windSpeed = 5.0f; 
		windDirection = glm::vec3(0.0f, 0.0f, 1.0f);

		creationRate = createRate;
		lifeSpan = lifeSp;
		numParticles = 0;
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

		colors = colorsInit;

		std::cout << "Initialized Particle Source" << std::endl;
	}

	ParticleSource(glm::vec3 p, glm::vec3 v, float m, float windSp,
					glm::vec3 windDir, float createRate = 30, float lifeSp = 100,
					float posVar = 0, float velVar = 0, float lifespVar = 0, float g = 9.8,
					float airDen = 1.225, float d = 0.47, float particleRad = 0.2,
					float colElast = 0, float colFrict = 0, std::vector<glm::vec3> colorsInit = std::vector<glm::vec3>{ glm::vec3(1.0f) }) {
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

		colors = colorsInit;
	}

	void UpdatePosition(glm::vec3 p) {
		position = p; 
	}
	 
	void UpdateVelocity(glm::vec3 v) {
		velocity = v;
	}
	
	void Update(float deltaTime, glm::vec3 p, glm::vec3 v, bool createNew );
	void Update(float deltaTime, glm::vec3 p, glm::vec3 v, float createRate, float lifeSp, float posVar, float velVar, float lifespVar, float particleRad, std::vector<glm::vec3> colors, bool createNew);

	void Update(float deltaTime, glm::vec3 p, glm::vec3 v, float m, float windSp,
		glm::vec3 windDir, float floor = -1, float createRate = 4, float lifeSp = 50.0f,
		float posVar = 0.05f, float velVar = 0.008f, float lifespVar = 0, float g = 0,
		float airDen = 5.225, float d = 0.1, float particleRad = 0.05,
		float colElast = -1, float colFrict = 0, std::vector<glm::vec3> colors = std::vector<glm::vec3>{ glm::vec3(1.0f) });
	void Draw(const glm::mat4& viewProjMtx, SurfaceShader* shader, GLuint program);
	void Draw(const glm::mat4& viewProjMtx, PartShader* shader, GLuint program);

	void CleanUp();
};

////////////////////////////////////////////////////////////////////////////////

#endif