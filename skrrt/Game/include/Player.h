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
	RealNumber makeupLevel; 

	// Current world position 
	// Current object's rotation 
	glm::vec3 current_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 initial_direction = glm::vec3(-1.0f, 0.0f, -0.0f);
	glm::vec3 current_direction = glm::vec3(-1.0f, 0.0f, -0.0f);

	glm::mat4 initial_car_transform;
	glm::mat4 animation_transform = glm::mat4(1.0f);

	glm::mat4 initial_crown_transform = glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)) * glm::scale(glm::vec3(0.8f, 0.8f, 0.8f));

	glm::mat4 initial_particle_transform = glm::translate(glm::vec3(0.6f, -0.4f, 0.0f)); 

	// Maybe a spawned position and rotation?

public: 

	Player() { player_node = NULL; has_crown = false; makeupLevel = 0; };
	Player(Node* car) { player_node = car; has_crown = false; makeupLevel = 0; };

	void moveCar(glm::vec3 dir, glm::vec3 up, glm::vec3 pos);
	void spinWheels(float rotationDegree);
	void bobCrown(float time);
	void updateParticles(float time);

	void setPlayer(Node* player) { 
		player_node = player; 
		initial_car_transform = player->childtransforms[0];
	};
	void setCrown(Node* crown) { crown_node = crown; };
	void setPosition(glm::vec3 position) { current_position = position; };
	void setMakeupLevel(RealNumber muLevel) { makeupLevel = muLevel; }; 
	void setPlayerTransform(glm::mat4 transform) { 
		//player_node->childtransforms[0] = initial_car_transform * transform; 
		//player_node->childtransforms[0] = transform * initial_car_transform; 

		//player_node->childtransforms[0] = glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
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

	bool getCrownStatus() {
		return has_crown;
	};


	RealNumber getMakeupLevel(void) { return makeupLevel; }
};

#endif
