#include "Player.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace glm;
void Player::transformCar(vec3 translation, vec3 scale, float rotationDegree,
	vec3 rotationAxis) {


}

void Player::spinWheels(float rotationDegree) {
	// From the car's car node 

	// For all child nodes that are tires
	for (Node* child: car_node->childnodes) {
		if (child->name.find("tire") != std::string::npos) {

			// Rotate tires by rotation degree
			// Edit model transform for each tire
			child->modeltransforms[0] = rotate(rotationDegree*float(M_PI)/180.0f, vec3(0.0, 0.0f, 1.0f)) * child->modeltransforms[0];
		}
	}
}