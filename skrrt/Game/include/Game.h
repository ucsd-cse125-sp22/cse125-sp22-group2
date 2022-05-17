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

class Game {
private:
	float game_time = 0; 

	// Add other parameters as needed
	Node* UI_root = nullptr;
	Node* drips = nullptr;
	glm::mat4 initial_drip_transform; 

public: 
	std::vector<int> scores; 
	std::vector<Player*> players; 

	Game(int numPlayers) {
		for (int i = 0; i < numPlayers; i++) {
			players.push_back(new Player());
			scores.push_back(0);
		}
	}

	void init(Node* ui_root) {
		UI_root = ui_root; 

		// Find drips
		for (Node* child : ui_root->childnodes[0]->childnodes) {
			if (child->name == "drips") {
				drips = child;
				std::cout << "drips name: " << child->name << std::endl;
			}
		}

		initial_drip_transform = drips->modeltransforms[0];
	}

	void updateDrips(int time); 

};


#endif
