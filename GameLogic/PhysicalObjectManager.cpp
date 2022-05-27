#include "PhysicalObjectManager.hpp"

PhysicalObjectManager::PhysicalObjectManager()
{
	this->objects = new vector<PhysicalObject*>();
	this->numMakeups = 0;
	// Note: gridMin, gridMax are based on the size of the scene
	this->uniformGrid = nullptr;
	// uniformGrid = createGrid(gridMin, gridMax, gridSizes);

	gameTime = cse125config::MATCH_LENGTH;
}

PhysicalObjectManager::~PhysicalObjectManager()
{
	for (unsigned int i = 0; i < objects->size(); i++) {
		delete objects->at(i);
	}
	delete objects;
	// delete uniformGrid;
}

void PhysicalObjectManager::startGame() {
	// TODO: this is going to be map dependent, could add a int parameter and switch statement if we want multiple maps
	// Create players
	// Create crown
	// Create makeup booths
	// Create walls and floors

	// Very basic map
	//createObject(oPlayer, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	//createObject(oPlayer, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	//createObject(oPlayer, glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	//createObject(oPlayer, glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	createObject(oPlayer, glm::vec3(21.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	createObject(oPlayer, glm::vec3(-21.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	createObject(oPlayer, glm::vec3(0.0f, 0.0f, 21.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	createObject(oPlayer, glm::vec3(0.0f, 0.0f, -21.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	createObject(oCrown, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	// Add 180 degrees to value in scene graph or maybe 180 - degrees from scene graph?
	createObject(oMakeup, glm::vec3(20.0f, 0.0, -20.0f), glm::vec3(cos(45.0f * float(M_PI) / 180.0f), 0.0f, sin(45.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	createObject(oMakeup, glm::vec3(-20.0f, 0.0, 20.0f), glm::vec3(cos(-135.0f * float(M_PI) / 180.0f), 0.0f, sin(-135.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	// Obstacles
	createObject(oWall, glm::vec3(4.0f, 0.0f, -28.0f), glm::vec3(cos(180.0f * float(M_PI) / 180.0f), 0.0f, sin(180.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f), oTireRack);
	createObject(oWall, glm::vec3(7.3f, 0.0f, 21.3f), glm::vec3(cos(160.0f * float(M_PI) / 180.0f), 0.0f, sin(160.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f), oTireRack);
	createObject(oWall, glm::vec3(-10.7f, 0.0f, 26.0f), glm::vec3(cos(210.0f * float(M_PI) / 180.0f), 0.0f, sin(210.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f), oTireRack);

	createObject(oWall, glm::vec3(-20.8f, 0.0f, -4.4f), glm::vec3(cos(180.0f * float(M_PI) / 180.0f), 0.0f, sin(180.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f), oCones);
	createObject(oWall, glm::vec3(-29.6f, 0.0f, -4.4f), glm::vec3(cos(180.0f * float(M_PI) / 180.0f), 0.0f, sin(180.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f), oCones);
	createObject(oWall, glm::vec3(20.8f, 0.0f, 4.4f), glm::vec3(cos(180.0f * float(M_PI) / 180.0f), 0.0f, sin(180.0f * float(M_PI) / 180.0f)), glm::vec3(0.0f, 1.0f, 0.0f), oCones);

	// Powerups
	createObject(oPowerup, glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	
	//createObject(oFloor, glm::vec3(3.0f, 0.0f, 6.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // UNCOMMENT TO TEST SLOPES!!!
	//createObject(oMakeup, glm::vec3(20.0f, 0.0, -20.0f), glm::vec3(0.0f, 0.0, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//createObject(oWall, glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//createObject(oWall, glm::vec3(2.0f, -8.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//createObject(oWall, glm::vec3(-2.0f, -8.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//createObject(oWall, glm::vec3(2.0f, -8.0f, -2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//createObject(oWall, glm::vec3(-2.0f, -8.0f, -2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//createObject(oWall, glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.0f));
	//createObject(oWall, glm::vec3(-4.0f, 0.0f, 0.0f), glm::vec3(0.0f));
	//createObject(oWall, glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f));
	//createObject(oWall, glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f));
}

void PhysicalObjectManager::endGame() {
	unsigned int winner = 0;
	float winner_score = 0.0f;
	for (unsigned int i = 0; i < objects->size(); i++) {
		if (objects->at(i)->type == oPlayer) {
			if (((ObjPlayer*)objects->at(i))->score > winner_score) {
				winner = i;
				winner_score = ((ObjPlayer*)objects->at(i))->score;
			}
		}
	}
	cout << "The game has ended!  Player " << winner + 1 << " has won!\n";
}

void PhysicalObjectManager::createObject(int objType, glm::vec3 pos, glm::vec3 dir, glm::vec3 up, int modifier) {
	unsigned int next_id = this->objects->size();

	switch (objType) {
	case (oPlayer):
		this->objects->push_back(new ObjPlayer(objects, next_id, pos, dir, up));
		break;
	case (oWall):
		switch (modifier) {
		case(oTireRack):
			this->objects->push_back(new ObjWall(objects, next_id, pos, TIRERACK_LENGTH, TIRERACK_WIDTH, TIRERACK_HEIGHT, dir, up));
			break;
		case(oCones):
			this->objects->push_back(new ObjWall(objects, next_id, pos, CONES_LENGTH, CONES_WIDTH, CONES_HEIGHT, dir, up));
			break;
		default:
			this->objects->push_back(new ObjWall(objects, next_id, pos, 1.0f, 1.0f, 1.0f, dir, up));
			break;
		}
		break;
	case (oFloor):
		this->objects->push_back(new ObjFloor(objects, next_id++, glm::vec3(0.0f, -1.0f, 0.0f), 8.0f, 8.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), true));
		this->objects->push_back(new ObjFloor(objects, next_id++, glm::vec3(12.0f, 1.0f, 0.0f), 8.0f, 8.0f, 1.0f, glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f)), glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f)), true));
		//this->objects->push_back(new ObjFloor(objects, next_id++, glm::vec3(0.0f, -2.0f, 16.0f), 12.0f, 1.0f, 12.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)), true));
		//this->objects->push_back(new ObjFloor(objects, next_id, glm::vec3(8.0f, -2.0f, 2.0f), 4.0f, 1.0f, 4.0f, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f,` 0.0f), true));
		break;
	case (oCrown):
		this->objects->push_back(new ObjCrown(objects, next_id, pos, dir, up));
		this->crownID = next_id;
		break;
	case (oMakeup):
		glm::vec3 offL = glm::vec3(glm::dot(glm::vec3(-dir.z, 0.0f, dir.x), BOOTH_WALL_L_OFFSET), BOOTH_WALL_L_OFFSET.y, glm::dot(dir, BOOTH_WALL_L_OFFSET));
		glm::vec3 offR = glm::vec3(glm::dot(glm::vec3(-dir.z, 0.0f, dir.x), BOOTH_WALL_R_OFFSET), BOOTH_WALL_R_OFFSET.y, glm::dot(dir, BOOTH_WALL_R_OFFSET));
		glm::vec3 offB = glm::vec3(glm::dot(glm::vec3(-dir.z, 0.0f, dir.x), BOOTH_WALL_L_OFFSET), BOOTH_BAR_OFFSET.y, glm::dot(dir, BOOTH_BAR_OFFSET));
		this->objects->push_back(new ObjWall(objects, next_id++, pos + offL, BOOTH_WALL_L_LENGTH, BOOTH_WALL_L_WIDTH, BOOTH_WALL_L_HEIGHT, dir, up));
		this->objects->push_back(new ObjWall(objects, next_id++, pos + offR, BOOTH_WALL_R_LENGTH, BOOTH_WALL_R_WIDTH, BOOTH_WALL_R_HEIGHT, dir, up));
		this->objects->push_back(new ObjWall(objects, next_id++, pos + offB, BOOTH_BAR_LENGTH, BOOTH_BAR_WIDTH, BOOTH_BAR_HEIGHT, dir, up));
		this->objects->push_back(new ObjMakeup(objects, next_id, numMakeups++, next_id - 1, pos, dir));
		break;
	case (oTrail):
		//this->objects->push_back(new ObjMakeup(objects, next_id, glm::vec3(0.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), false));
		break;
	case (oPowerup):
		this->objects->push_back(new ObjPowerup(objects, next_id, vector<glm::vec3>{pos}, dir, up));
		//this->objects->push_back(new ObjMakeup(objects, next_id, glm::vec3(0.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), false));
		break;
	default:
		break;
	}
	// this->objects->push_back(new PhysicalObject(objects, glm::vec3(0.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), next_id, true));
}

void PhysicalObjectManager::step(bool* matchInProgress) {
	if (gameTime > 0.0f) {
		gameTime -= 1.0f / cse125config::TICK_RATE;
		if (gameTime <= 0.0f) {
			gameTime = 0.0f;
			endGame();
			*matchInProgress = false;
		}
	}

	vector<unsigned int> deletions{};
	for (unsigned int i = 0; i < objects->size(); i++) {
		if (objects->at(i)->type == oPlayer) {
			((ObjPlayer*)objects->at(i))->step(gameTime);
		}
		else if (objects->at(i)->type == oMakeup) {
			((ObjMakeup*)objects->at(i))->step();
		}
		else if (objects->at(i)->type == oTrail) {
			((ObjTrail*)objects->at(i))->step();
			if (((ObjTrail*)(objects->at(i)))->life <= 0.0f) {
				deletions.push_back(i);
			}
			
		}
		else if (objects->at(i)->type == oPowerup) {
			((ObjPowerup*)objects->at(i))->step();
		}
	}


	for (int i = deletions.size() - 1; i >= 0; i--) {
		objects->at(deletions.at(i)) = objects->at(objects->size() - 1);
		objects->pop_back();
	}
}


// TODO: Everything below this line is not used yet
vector<vector<int>*>* PhysicalObjectManager::createGrid(glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes) {
	glm::vec3 numGrids = (gridMax - gridMin) / gridSizes;
	vector<vector<int>*>* uniformGrid = new vector<vector<int>*>(numGrids.x * numGrids.y * numGrids.z);
	for (unsigned int i = 0; i < uniformGrid->size(); i++) {
		(*uniformGrid)[i] = new vector<int>();
	}

	for (unsigned int i = 0; i < objects->size(); i++) {
		PhysicalObject* obj = (*objects)[i];

		// Find lowest and highest potential cells in each axis
		glm::vec3 lowestCell = (obj->boundingBox.bbMin - gridMin) / gridSizes;
		glm::vec3 highestCell = (obj->boundingBox.bbMax - gridMin) / gridSizes;

		for (unsigned int xi = lowestCell.x; xi <= highestCell.x; xi++) {
			for (unsigned int yi = lowestCell.y; yi <= highestCell.y; yi++) {
				for (unsigned int zi = lowestCell.z; zi <= highestCell.z; zi++) {
					// For now just add, could add a collision check though
					unsigned int cellNum = (xi * (unsigned int)numGrids.x * (unsigned int)numGrids.y) + (yi * (unsigned int)numGrids.y) + zi;
					(*uniformGrid)[cellNum]->push_back(obj->id);
					obj->boundingBox.gridCells.push_back(cellNum);
				}
			}
		}
	}

	return uniformGrid;
}

void PhysicalObjectManager::removeFromGrid(PhysicalObject* obj) {
	BoundingBox& boundingBox = obj->boundingBox;
	int id = obj->id;

	for (unsigned int i = 0; i < boundingBox.gridCells.size(); i++) {
		vector<int>* cell = (*uniformGrid)[boundingBox.gridCells[i]];
		for (int j = cell->size() - 1; j >= 0; j--) {
			if (cell->at(j) == id) {
				cell->at(j) = cell->at(cell->size() - 1);
				cell->pop_back();
			}
		}
	}

	boundingBox.gridCells = vector<int>();
}

void PhysicalObjectManager::addToGrid(PhysicalObject* obj, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes) {
	glm::vec3 numGrids = (gridMax - gridMin) / gridSizes;
	vector<vector<int>*>* uniformGrid = new vector<vector<int>*>(numGrids.x * numGrids.y * numGrids.z);
	for (unsigned int i = 0; i < uniformGrid->size(); i++) {
		(*uniformGrid)[i] = new vector<int>();
	}

	// Find lowest and highest potential cells in each axis
	glm::vec3 lowestCell = (obj->boundingBox.bbMin - gridMin) / gridSizes;
	glm::vec3 highestCell = (obj->boundingBox.bbMax - gridMin) / gridSizes;

	for (unsigned int xi = lowestCell.x; xi <= (unsigned int)highestCell.x; xi++) {
		for (unsigned int yi = lowestCell.y; yi <= (unsigned int)highestCell.y; yi++) {
			for (unsigned int zi = lowestCell.z; zi <= (unsigned int)highestCell.z; zi++) {
				// For now just add, could add a collision check though
				unsigned int cellNum = (xi * (unsigned int)numGrids.x * (unsigned int)numGrids.y) + (yi * (unsigned int)numGrids.y) + zi;
				(*uniformGrid)[cellNum]->push_back(obj->id);
				obj->boundingBox.gridCells.push_back(cellNum);
			}
		}
	}
}
