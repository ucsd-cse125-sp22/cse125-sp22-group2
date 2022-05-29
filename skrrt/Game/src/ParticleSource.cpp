#include "ParticleSource.h"

/*
namespace this_sucks {
	std::default_random_engine gen;
	std::uniform_real_distribution<float> distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);;

	//gen = std::default_random_engine(); 
}
*/

//using namespace this_sucks;

void ParticleSource::Update(float deltaTime, glm::vec3 p, glm::vec3 v, bool createNew) {
	Update(deltaTime, p, v, 0.01, 0, glm::vec3(0.0f), -6, (createNew ? 1 : 0));
}
void ParticleSource::Update(float deltaTime, glm::vec3 p, glm::vec3 v, float createRate, float lifeSp, float posVar, float velVar, float lifespVar, float particleRad, glm::vec3 color, bool createNew) {
	Update(deltaTime, p, v, 0.01, 0, glm::vec3(0.0f), -6, (createNew ? createRate : 0), lifeSpan, posVar, velVar, lifespVar, 0.0f, 5.225f, 0.1f, particleRad, -1, 0, color);
}

/*
void ParticleSource::Update(float deltaTime, glm::vec3 p, glm::vec3 v, float m, float windSp,
							glm::vec3 windDir, float floor = -1, float createRate = 1, float lifeSp = 100,
							float posVar = 0, float velVar = 0, float lifespVar = 0, float g = 9.8,
							float airDen = 1.225, float d = 0.47, float particleRad = 0.01,
							float colElast = 0, float colFrict = 0) {
							*/
void ParticleSource::Update(float deltaTime, glm::vec3 p, glm::vec3 v, float m, float windSp,
							glm::vec3 windDir, float floor, float createRate, float lifeSp,
							float posVar, float velVar, float lifespVar, float g,
							float airDen, float d, float particleRad,
							float colElast, float colFrict, glm::vec3 color) {

	//std::cout << "Called Particle Source Update" << std::endl;

	// Update values 
	position = p; 
	velocity = v; 
	mass = m; 
	windSpeed = windSp; 
	windDirection = windDir;
	creationRate = createRate;
	lifeSpan = lifeSp;
	positionVariance = posVar; 
	velocityVariance = velVar; 
	lifespanVariance = lifespVar; 
	gravity = g; 
	airDensity = airDen; 
	drag = d; 
	particleRadius = particleRad; 
	collisionElasticity = colElast; 
	collisionFriction = colFrict;
	glm::vec3 n = glm::vec3(0.0f, 1.0f, 0.0f);

	std::normal_distribution<float> distribution{ 0.0, 1.0 };

	// Determine how many new particles to create this frame
	float num = deltaTime * creationRate + roundOffError;
	int newParticles = int(num);
	roundOffError = num - float(newParticles);

	// Create particles
	for (int i = 0; i < newParticles && numParticles < MAX_PARTICLES; i++) {

		// Add variance in position, velocity, and lifespan
		glm::vec3 deltaPos = glm::vec3(position.x + distribution(gen) * positionVariance, position.y + distribution(gen) * positionVariance, position.z + distribution(gen) * positionVariance);
		glm::vec3 deltaVel = glm::vec3(velocity.x + distribution(gen) * velocityVariance, velocity.y + distribution(gen) * velocityVariance, velocity.z + distribution(gen) * velocityVariance);

		float deltaLifeSpan = lifeSpan + distribution(gen) * lifespanVariance;

		//std::cout << "Varied position: " << deltaPos.x << ", " << deltaPos.y << ", " << deltaPos.z << std::endl;

		float rad = particleRad;
		if (particleRad > 0.1f) {
			rad += distribution(gen) * 0.02f;
		}
		particles[numParticles] = new Particle(deltaPos, deltaVel, glm::vec3(0.0f), mass, false, deltaLifeSpan, rad, color);

		float random = ((float)rand() / INT_MAX);

		// First frame adjustments 
		particles[numParticles]->Integrate(deltaTime * random);

		numParticles++;
	}

	// Clean up expired particles 
	CleanUp();

	// Apply forces on particles for deltaTime
	for (int i = 0; i < numParticles; i++) {
		Particle* p = particles[i];

		// Gravity
		p->ApplyForce(glm::vec3(0, -gravity * p->getMass(), 0));

		// Aerodynamic force
		//float area = 3.1415927 * p->getRadius() * p->getRadius();
		float area = p->getRadius() * p->getRadius();
		glm::vec3 v = p->getVelocity() - (windSpeed * glm::normalize(windDir));
		glm::vec3 dragDir = -glm::normalize(v);

		glm::vec3 f_drag = 0.5f * airDensity * glm::length(v) * glm::length(v) * drag * area * dragDir;

		if (isnan(f_drag.x)) {
			f_drag = glm::vec3(0.0f);
		}

		p->ApplyForce(f_drag);
	}

	// Integrate for position and velocity based on forces
	for (int i = 0; i < numParticles; i++) {
		particles[i]->Integrate(deltaTime);
	}

	// Apply constraints (collisions)
	// For each particle: if intersecting, push to legal position & adjust velocity

	// Check if below floor 
	for (int i = 0; i < numParticles; i++) {
		Particle* p = particles[i];

		// In contact with floor
		if (p->getPosition().y <= floor) {
			glm::vec3 initialP = p->getPosition() - deltaTime * p->getVelocity();
			glm::vec3 initialV = p->getVelocity();

			// Compute impulse 
			float v_close = glm::dot(p->getVelocity(), n);

			glm::vec3 impulse = -(1 + collisionElasticity) * p->getMass() * v_close * n;

			// Correct velocity 
			glm::vec3 V = p->getVelocity();
			glm::vec3 deltaV = (1.0f / p->getMass()) * impulse;
			glm::vec3 newV = glm::vec3(V.x + deltaV.x, V.y + deltaV.y, V.z + deltaV.z);

			p->setVelocity(newV);

			// Compute frictional velocity 
			glm::vec3 vnorm = glm::dot(p->getVelocity(), n) * n;
			glm::vec3 vtan = p->getVelocity() - vnorm;
			impulse = -collisionFriction * glm::length(impulse) * glm::normalize(vtan);

			// Correct velocity 
			V = p->getVelocity();
			deltaV = (1.0f / p->getMass()) * impulse;

			// Check if change in velocity is going to change tangential velocity's direction 
			if (abs(deltaV.x) > abs(vtan.x)) {
				deltaV.x = -vtan.x;
			}
			if (abs(deltaV.y) > abs(vtan.y)) {
				deltaV.y = -vtan.y;
			}
			if (abs(deltaV.z) > abs(vtan.z)) {
				deltaV.z = -vtan.z;
			}

			newV = glm::vec3(V.x + deltaV.x, V.y + deltaV.y, V.z + deltaV.z);

			p->setVelocity(newV);

			// Push particle to legal position
			float timeToCollide = (initialP.y - floor) / initialV.y;
			glm::vec3 collideP = initialP + timeToCollide * initialV;
			glm::vec3 bounceP = collideP + (deltaTime - timeToCollide) * newV;

			if (bounceP.y <= floor) {
				bounceP.y = floor + 0.01;
			}

			p->setPosition(bounceP);
		}
	}
}

void ParticleSource::Draw(const glm::mat4& viewProjMtx, GLuint shader) {

	//std::cout << "Print Particle Source" << std::endl;

	// Draw all particles 
	for (int i = 0; i < numParticles; i++) {
		particles[i]->draw(viewProjMtx, shader);
	}
}

void ParticleSource::CleanUp() {

	// Iterate through particles and remove expired particles
	for (int i = 0; i < numParticles; i++) {
		if (particles[i] != NULL && particles[i]->getLifespan() <= 0) {
			// Delete particle there 
			delete(particles[i]);

			// Replace with last particle in list 
			particles[i] = particles[numParticles - 1];
			particles[numParticles - 1] = NULL;

			i--;
			numParticles--;
		}
	}
}