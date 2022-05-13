#include "Player.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace glm;

// reserving the up vector for potential ramp movements
void Player::moveCar(vec3 dir, vec3 up, vec3 pos) {

	// rotate the car to face direction it will move
	mat4 r = inverse(lookAt(vec3(0.0f), dir, up));

	// translate the car based on offset
	mat4 t = translate(pos);

	mat4 s = scale(vec3(0.5f, 0.5f, 0.5f));
	mat4 correction = rotate(-1.0f * radians(90.0f), vec3(0.0f, 1.0f, 0.0f));

	player_node->childtransforms[0] = t * s * correction * r;

	//current_position = glm::vec3(player_node->childtransforms[0][3][0], 
		//player_node->childtransforms[0][3][1], player_node->childtransforms[0][3][2]);
	current_position = glm::vec3(player_node->childtransforms[0][0][3], 
		player_node->childtransforms[0][1][3], player_node->childtransforms[0][2][3]);
}

void Player::spinWheels(float rotationDegree) {
	// From the car's car node 
	Node* car_node = player_node->childnodes[0];

	// For all child nodes that are tires
	for (Node* child: car_node->childnodes) {
		if (child->name.find("tire") != std::string::npos) {

			// Rotate tires by rotation degree
			// Edit model transform for each tire
			child->modeltransforms[0] = rotate(rotationDegree*float(M_PI)/180.0f, vec3(0.0, 0.0f, 1.0f)) * child->modeltransforms[0];
		}
	}
}

void Player::bobCrown(float time) {

	Node* car_node = player_node->childnodes[0];

	// Change position of the crown based on time 
	mat4 offset = translate(vec3(0.0f, sin(time * float(M_PI) / 1400.0f) / 4.0f, 0.0f));

	for (Node* child : car_node->childnodes) {
		if (child->name.find("crown") != std::string::npos) {
			child->modeltransforms[0] = offset * initial_crown_transform;
		}
	}

}

void Player::updateParticles(float time) {
	Node* car_node = player_node->childnodes[0]; 

	// Take car's transform and transform the offset of the particles 

	//mat4 new_particle_transform = initial_particle_transform * player_node->childtransforms[0]; 
	//mat4 new_particle_transform = player_node->childtransforms[0] * initial_particle_transform; 
	//vec3 position = vec3(new_particle_transform[3][0], new_particle_transform[3][1], new_particle_transform[3][2]);

	vec3 position = vec3(0.6f, -0.4f, 0.0f) + current_position;
	vec3 velocity = vec3(0.06f, 0.0f, -0.0f); 

	// Call update for the particle source 
	for (Node* child : car_node->childnodes) {
		if (child->isParticleSource) {
			child->particles->Update(time, position, velocity);
		}
	}
}