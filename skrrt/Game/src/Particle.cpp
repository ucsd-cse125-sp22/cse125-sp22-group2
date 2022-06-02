#include "Particle.h"

Particle::Particle(glm::vec3& p, glm::vec3& v, glm::vec3& f, float m, bool fix, float lifesp, float rad, Model* particleModel, glm::vec3 c) {
	glm::vec3 pos = p;
	OriginalPosition = p;
	Position = OriginalPosition; 
	Velocity = v; 
	Force = f; 
	Mass = m;
	fixed = fix;
	normal = glm::vec3(0);
	lifespan = lifesp;
	radius = rad;
	color = c;

	model = particleModel;
}

void Particle::ApplyPositionChange(float x, float y, float z) {
	if (fixed) {
		Position.x = OriginalPosition.x + x; 
		Position.y = OriginalPosition.y + y; 
		Position.z = OriginalPosition.z + z;
	}
}

void Particle::draw(const glm::mat4& viewProjMtx, GLuint shader) {
	// Stop rendering until clean up
	if (lifespan > 0) {
		//Cube cube = Cube(Position, glm::vec3(Position.x + 0.02, Position.y + 0.02, Position.z + 0.02));

		if (model == nullptr) {
			ParticleCube cube = ParticleCube(Position, glm::vec3(Position.x + radius, Position.y + radius, Position.z + radius));
			cube.setColor(color);

			cube.draw(viewProjMtx, shader);
		}
		else {
			Node* node = new Node("particle");
			node->models.push_back(model); 
			node->modeltransforms.push_back(glm::translate(Position) * glm::scale(0.1f * glm::vec3(1.0f)));

			glUseProgram(shader); 
			((Obj*)(node->models[0]->geometry))->draw();
		}
	}
}

void Particle::setNormal(glm::vec3 n) {
	normal = n;
}

void Particle::addNormal(glm::vec3 n) {
	normal.x += n.x; 
	normal.y += n.y; 
	normal.z += n.z;
}

void Particle::normalizeNormal() {
	normal = glm::normalize(normal);
}