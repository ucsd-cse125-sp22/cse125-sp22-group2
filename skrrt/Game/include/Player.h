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
#include "ParticleSource.h"
#include "../../../Definitions.hpp"

#ifndef __PLAYER_H__
#define __PLAYER_H__

class Player {
private:
	// player node 
	Node* player_node; 

	// crown node 
	Node* crown_node;
	bool has_crown;

	// Player's makeup level 
	RealNumber makeupLevel = 0.0f; 
	
	// Player's score 
	RealNumber score = 0;

	// Player's powerup info
	Node* blowdryer_node;
	bool using_powerup = false;
	bool has_powerup = false;

	// Player's state
	RealNumber iframes = 0.0f;

	// Current world position 
	// Current object's rotation 
	glm::vec3 current_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 initial_direction = glm::vec3(-1.0f, 0.0f, -0.0f);
	glm::vec3 current_direction = glm::vec3(-1.0f, 0.0f, -0.0f);
	RealNumber current_speed = 0.0f;

	glm::mat4 initial_car_transform;
	glm::mat4 animation_transform = glm::mat4(1.0f);

	glm::mat4 initial_crown_transform = glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)) * glm::scale(glm::vec3(0.8f, 0.8f, 0.8f));

	glm::mat4 initial_particle_transform = glm::translate(glm::vec3(0.6f, -0.4f, 0.0f)); 


public: 
	Player() { player_node = NULL; has_crown = false; makeupLevel = 100.0f; };
	Player(Node* car) { player_node = car; has_crown = false; makeupLevel = 100.0f; };

	void moveCar(glm::vec3 dir, glm::vec3 up, glm::vec3 pos);
	void spinWheels(float rotationDegree);
	void bobCrown(float time);
	void updateParticles(float time, std::vector<glm::vec3> colors);

	void setPlayer(Node* player) { 
		player_node = player; 
		initial_car_transform = player->childtransforms[0];
	};
	void setCrown(Node* crown) { crown_node = crown; };
	void setBlowdryer(Node* blowdryer) { blowdryer_node = blowdryer; };
	void setPosition(glm::vec3 position) { current_position = position; };
	void setSpeed(RealNumber speed) { current_speed = speed; }
	void setMakeupLevel(RealNumber muLevel) { makeupLevel = muLevel; }; 
	void setPlayerScore(RealNumber s) { score = s; };
	void setInvincibility(RealNumber inv) { 
		iframes = inv; 

		std::stack < Node* > dfs_stack;
		dfs_stack.push(player_node);
		if (has_crown) {
			dfs_stack.push(crown_node);
		}
		while (!dfs_stack.empty()) {
			Node* cur = dfs_stack.top();
			dfs_stack.pop();
			if (cur->isParticleSource) {
				continue;
			}
			cur->iframes = inv;
			for (Node* child : cur->childnodes) {
				dfs_stack.push(child);
			}
		}
	}
	void setPlayerTransform(glm::mat4 transform) { 
		animation_transform = transform;

		/*
		std::cout << "car transformation : " << std::endl; 
		glm::mat4 car_transform = initial_car_transform * transform;
		std::cout << car_transform[0][0] << " " << car_transform[0][1] << " " << car_transform[0][2] << " " << car_transform[0][3] << std::endl;
		std::cout << car_transform[1][0] << " " << car_transform[1][1] << " " << car_transform[1][2] << " " << car_transform[1][3] << std::endl;
		std::cout << car_transform[2][0] << " " << car_transform[2][1] << " " << car_transform[2][2] << " " << car_transform[2][3] << std::endl;
		std::cout << car_transform[3][0] << " " << car_transform[3][1] << " " << car_transform[3][2] << " " << car_transform[3][3] << std::endl;
		*/

	};

	void setCrownStatus(bool status) { 
		has_crown = status; 
		crown_node->visible = status;
	};

	void setHasPowerup(bool status) {
		has_powerup = status;
	};

	void setUsingPowerup(bool status) {
		using_powerup = status;
		blowdryer_node->visible = status | has_powerup;
	};

	bool getCrownStatus() {
		return has_crown;
	};

	bool getHasPowerup() {
		return has_powerup;
	};

	bool getUsingPowerup() {
		return using_powerup;
	};

	glm::vec3 getPosition() {
		return current_position;
	};

	RealNumber getSpeed() { return current_speed; }


	RealNumber getMakeupLevel(void) { return makeupLevel; }
	RealNumber getScore(void) { return score; }
	RealNumber getInvincibility() { return iframes; }
};

#endif
