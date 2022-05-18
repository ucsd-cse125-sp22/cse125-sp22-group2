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

public: 
	std::vector<int> scores; 
	std::vector<Player*> players; 

	Game(int numPlayers) {
		for (int i = 0; i < numPlayers; i++) {
			players.push_back(new Player());
			scores.push_back(0);
		}
	}

};


#endif
