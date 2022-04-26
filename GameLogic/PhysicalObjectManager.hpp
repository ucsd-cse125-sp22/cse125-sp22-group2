#pragma once

#include "core.h"
#include "BoundingBox.hpp"
#include "PhysicalObject.hpp"

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

	PhysicalObjectManager();
	~PhysicalObjectManager();

	// Create an instance
	// void createObject(object type, pos, dir);
	void createObject();
	
	// These will not be used for now
	vector<vector<int>*>* createGrid(glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
	void removeFromGrid(PhysicalObject* obj);
	void addToGrid(PhysicalObject* obj, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes);
};