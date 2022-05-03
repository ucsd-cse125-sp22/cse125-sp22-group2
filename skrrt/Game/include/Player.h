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

	// crown node 
	Node* crown_node;

	// Score
	int score;

	bool has_crown;

	// Current world position 
	// Current object's rotation 
	glm::vec3 current_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 current_direction = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::mat4 initial_crown_transform = glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)) * glm::scale(glm::vec3(0.8f, 0.8f, 0.8f));

	// Maybe a spawned position and rotation?

public: 

	Player() { player_node = NULL; score = 0; has_crown = false; };
	Player(Node* car) { player_node = car; score = 0; has_crown = false; };

	void moveCar(glm::vec3 dir, glm::vec3 up, glm::vec3 pos);
	void spinWheels(float rotationDegree);
	void bobCrown(float time);

	void setPlayer(Node* player) { player_node = player; };
	void setCrown(Node* crown) { crown_node = crown; };
	void setPosition(glm::vec3 position) { current_position = position; };

	void setCrownStatus(bool status) { 
		has_crown = status; 
		crown_node->visible = status;
	};

	bool getCrownStatus() {
		return has_crown;
	};
	
	void updateScore(int s) { score = s; };
	int getScore() { return score; };

};

#endif
