#include "PhysicalObjectManager.hpp"

PhysicalObjectManager::PhysicalObjectManager()
{
	vector<PhysicalObject*>* objects = new vector<PhysicalObject*>();
	// Note: gridMin, gridMax are based on the size of the scene
	// uniformGrid = createGrid(gridMin, gridMax, gridSizes);
}

PhysicalObjectManager::~PhysicalObjectManager()
{
	for (unsigned int i = 0; i < objects->size(); i++) {
		delete objects->at(i);
	}
	delete objects;
	// delete uniformGrid;
}

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
		for (unsigned int j = cell->size() - 1; j >= 0; j--) {
			if (cell->at(j) == id) {
				cell->at(j) == cell->at(cell->size() - 1);
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
