#pragma once

#include "core.h"
#include "BoundingBox.hpp"
#include "PhysicalObject.hpp"
#include "ObjPlayer.hpp"
#include "ObjWall.hpp"
#include "ObjFloor.hpp"
#include "ObjCrown.hpp"
#include "ObjMakeup.hpp"
#include "ObjTrail.hpp"
#include "ObjPowerup.hpp"

enum Obstacles { oTireRack, oCones, oPillar, oCake, oFlowers };

class PhysicalObjectManager
{
private:

public:
	// Vector that contains every physical object in the game world
	vector<PhysicalObject*>* objects;
	// Can eventually be used to speed up collision checking, it should work but I am not using it for now
	vector<vector<int>*>* uniformGrid;
	// A list of vanancies in the objects vector, we shouldn't need this since we probably won't be deleting objects
	// vector<unsigned int> vacancies;

	// Index for the current sound effect
	int audioIndex = 0;
	// Array of audio triggers
	//cse125framing::AudioTrigger audioTriggers[cse125constants::MAX_NUM_SOUNDS];

	// How much of the match remains
	float gameTime;
	// How many makeup booths there are
	unsigned int numMakeups;
	// ID for the loose crown
	unsigned int crownID;
	// ID for the first powerup
	unsigned int powerupIDOffset;

	PhysicalObjectManager();
	~PhysicalObjectManager();

	// Initialize map and objects
	void startGame();

	// End the game
	void endGame(int* winnerId);

	// Create an instance
	void createObject(int objType, glm::vec3 pos, glm::vec3 dir, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), int modifier = 0);

	// Update general game state once per tick
	void step(bool* matchInProgress, int* winnerId);
	
	// These will not be used for now
	vector<vector<int>*>* createGrid(glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
	void removeFromGrid(PhysicalObject* obj);
	void addToGrid(PhysicalObject* obj, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
};