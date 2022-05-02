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
	// player node 
	Node* player_node; 

	// Score
	int score;

	// Current world position 
	// Current object's rotation 
	glm::vec3 current_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 current_direction = glm::vec3(0.0f, 0.0f, -1.0f);

	// Maybe a spawned position and rotation?

public: 
	Player() { player_node = NULL; score = 0; };
	Player(Node* car) { player_node = car; score = 0; };

	void moveCar(glm::vec3 dir, glm::vec3 up, glm::vec3 pos);
	void spinWheels(float ratationDegree);

	void setPlayer(Node* player) { player_node = player; };
	void setPosition(glm::vec3 position) { current_position = position; };
	
	void updateScore(int s) { score = s; };
	int getScore() { return score; };

};

#endif
