#include "Player.h"

void Player::transformCar(glm::vec3 translation, glm::vec3 scale, float rotationDegree, 
	glm::vec3 rotationAxis) {


}

void Player::spinWheels(float rotationDegree) {
	// From the car's car node 

	// For all child nodes that are tires
	for (Node* child: car_node->childnodes) {
		if (child->name.find("tire") != std::string::npos) {
			// Rotate tires by rotation degree
			//rotate(rotationDegree*float(M_PI)/180.0f, vec3(0.0f, 0.0f, 1.0f)) 
			
		}
	}
}