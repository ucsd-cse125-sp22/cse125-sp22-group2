#include "Player.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace glm;

// reserving the up vector for potential ramp movements
void Player::moveCar(vec3 target, vec3 up, vec3 offset) {

	// rotate the car to face direction it will move
	//mat4 rotate = inverse(lookAt(current_position, target, glm::vec3(0.0f, 1.0f, 0.0f)));
	mat4 rotate = inverse(lookAt(current_position, target, glm::vec3(0.0f, 1.0f, 0.0f)));

	// translate the car based on offset
	mat4 translate = mat4(1.0f);

	player_node->childtransforms[0] = translate * rotate;

	current_position = glm::vec3(player_node->childtransforms[0][3][0], 
		player_node->childtransforms[0][3][1], player_node->childtransforms[0][3][2]);
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