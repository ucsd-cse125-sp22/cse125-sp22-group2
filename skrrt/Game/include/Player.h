#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>

#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <stack>

#include "Scene.h"

#ifndef __PLAYER_H__
#define __PLAYER_H__

class Player {
private:
	// Car node 
	Node* car_node; 

	// Score
	int score;

	// Current world position 
	// Current object's rotation 

	// Maybe a spawned position and rotation?

public: 
	Player() { car_node = NULL; score = 0; };
	Player(Node* car) { car_node = car; score = 0; };

	void transformCar(glm::vec3 translation, glm::vec3 scale, float rotationDegree, glm::vec3 rotationAxis);
	void spinWheels(float ratationDegree);

	void setCar(Node* car) { car_node = car; };
	
	void updateScore(int s) { score = s; };
	int getScore() { return score; };

};

#endif
