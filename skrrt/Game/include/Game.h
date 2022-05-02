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
	int p0_score = 0, p1_score = 0, p2_score = 0, p3_score = 0; 
	float game_time = 0; 

	// Add other parameters as needed

public: 
	Player *p0, *p1, *p2, *p3;

	Game(Player* player0, Player* player1, Player* player2, Player* player3) 
		{ p0 = player0; p1 = player1; p2 = player2; p3 = player3; };
};


#endif
