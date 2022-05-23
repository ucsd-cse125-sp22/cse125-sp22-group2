#pragma once

#include "core.h"
#include "BoundingBox.hpp"
#include "PhysicalObject.hpp"
#include "ObjPlayer.hpp"
#include "ObjWall.hpp"
#include "ObjFloor.hpp"
#include "ObjCrown.hpp"
#include "ObjMakeup.hpp"

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

	// How much of the match remains
	float gameTime;

	PhysicalObjectManager();
	~PhysicalObjectManager();

	// Initialize map and objects
	void startGame();

	// End the game
	void endGame();

	// Create an instance
	void createObject(int objType, glm::vec3 pos, glm::vec3 dir, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));


	/**
	 * @brief Update general game state once per tick 
	 *
	 * @param matchInProgress will be set to false if the game has ended, unchanged otherwise
	 */
	void step(bool* matchInProgress);
	
	// These will not be used for now
	vector<vector<int>*>* createGrid(glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
	void removeFromGrid(PhysicalObject* obj);
	void addToGrid(PhysicalObject* obj, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
};