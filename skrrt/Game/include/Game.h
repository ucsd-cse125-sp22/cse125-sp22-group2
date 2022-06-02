#define GLM_FORCE_RADIANS
#define _USE_MATH_DEFINES
#ifndef __GAME_H__
#define __GAME_H__

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>

#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <stack>

#include "Player.h"
#include "Geometry.h"
#include "Material.h"
#include "Model.h"
#include "Animation.h"

#include "../../../Constants.hpp"
#include "../../../Definitions.hpp"

class Game {
private:
	float game_time = 0;

	// Add other parameters as needed
	Node* UI_root = nullptr;
	Node* drips = nullptr;
	glm::mat4 initial_drip_transform;

	Node* makeup_status_bar = nullptr; 
	glm::mat4 initial_makeup_transform; 

	std::vector<Node*> makeup_gate_arms;
	std::vector<glm::mat4> initial_arm_transforms;

	std::vector<Node*> lipsticks;
	std::vector<glm::mat4> initial_lipstick_transforms;

	std::vector<Node*> mascaras;
	std::vector<glm::mat4> initial_mascaras_transforms;

	std::vector<Node*> powder_brushes;
	std::vector<glm::mat4> initial_powder_transforms;

	//Animation gateAnimation;
	std::map <std::string, Animation*> animations; 
	void applyAnimations(); 

public: 
	std::vector<int> scores; 
	std::vector<Player*> players; 

	Game(int numPlayers) {
		for (int i = 0; i < numPlayers; i++) {
			players.push_back(new Player());
			scores.push_back(0);
		}
	}

	~Game() {
		for (Player* p : players) {
			delete(p); 
		}
	}

	void init(Node* world, Node* ui_root) {
		UI_root = ui_root; 


		// Find drips
		for (Node* child : ui_root->childnodes[0]->childnodes) {
			if (child->name == "drips") {
				drips = child;
			}

			if (child->name == "white_bar") {
				makeup_status_bar = child;
			}
		}

		initial_drip_transform = drips->modeltransforms[0];
		initial_makeup_transform = makeup_status_bar->modeltransforms[0]; 

		// Find makeup gate arms
		for (Node* child : world->childnodes) {

			// Save makeup gate arms and lipstick objects
			if (child->name.find("makeup_station") != std::string::npos) {
				// makeup gate arm 
				makeup_gate_arms.push_back(child->childnodes[0]); 
				initial_arm_transforms.push_back(child->childnodes[0]->modeltransforms[0]);

				// lipstick 
				lipsticks.push_back(child->childnodes[1]); 
				initial_lipstick_transforms.push_back(child->childnodes[1]->modeltransforms[0]);

				// mascaras
				mascaras.push_back(child->childnodes[2]);
				initial_mascaras_transforms.push_back(child->childnodes[2]->modeltransforms[0]);

				// powder brush 
				powder_brushes.push_back(child->childnodes[3]);
				initial_powder_transforms.push_back(child->childnodes[3]->modeltransforms[0]);
			}
		}

		// Read in animations 
		parseGateAnimation(); 
		parseCarCollisionAnimation();
	}

	void updateTime(RealNumber t) { game_time = t; }
	float getTime() { return game_time; };

	void updateDrips(int time, RealNumber makeupLevel); 
	void updateMakeupStatusBar(int time, RealNumber makeupLevel); 
	void updateAnimations(); 

	void parseGateAnimation();
	void triggerGateAnimation(int gateNum);

	void parseCarCollisionAnimation();
	void triggerCarCollisionAnimation(int playerNum);
};


#endif
