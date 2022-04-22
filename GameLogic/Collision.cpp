#include "Collision.hpp"

using namespace collision;

////////////////////////////////////////////////////////////////////////////////

bool collision::checkCollision(BoundingBox a, BoundingBox b) {

	// SAT collision test
	// Get all the axes along which we check for a gap between the objects
	vector<glm::vec3> axes;
	axes.push_back(a.aVec);
	axes.push_back(a.bVec);
	axes.push_back(a.cVec);
	axes.push_back(b.aVec);
	axes.push_back(b.bVec);
	axes.push_back(b.cVec);
	axes.push_back(glm::cross(a.aVec, b.aVec));
	axes.push_back(glm::cross(a.aVec, b.bVec));
	axes.push_back(glm::cross(a.aVec, b.cVec));
	axes.push_back(glm::cross(a.bVec, b.aVec));
	axes.push_back(glm::cross(a.bVec, b.bVec));
	axes.push_back(glm::cross(a.bVec, b.cVec));
	axes.push_back(glm::cross(a.cVec, b.aVec));
	axes.push_back(glm::cross(a.cVec, b.bVec));
	axes.push_back(glm::cross(a.cVec, b.cVec));
	
	// Go through each of the axes we found above
	for (unsigned int i = 0; i < axes.size(); i++) {
		float a_min = 0.f;
		float a_max = 0.f;
		float b_min = 0.f;
		float b_max = 0.f;

		// Get the min/max for a
		for (unsigned int j = 0; j < a.vertices.size(); j++) {
			// Project onto the normal
			float proj = glm::dot(axes[i], a.vertices[j]);
			// Set min
			if (i == 0 || proj < a_min) {
				a_min = proj;
			}
			// Set max
			if (i == 0 || proj > a_max) {
				a_max = proj;
			}
		}

		// Get the min/max for b
		for (unsigned int j = 0; j < b.vertices.size(); j++) {
			// Project onto the normal
			float proj = glm::dot(axes[i], b.vertices[j]);
			// Set min
			if (i == 0 || proj < b_min) {
				b_min = proj;
			}
			// Set max
			if (i == 0 || proj > b_max) {
				b_max = proj;
			}
		}

		// Check for a gap
		if (a_max < b_min || a_min > b_max) {
			// Found an axis where they don't overlap, so no collision
			return false;
		}
	}

	// We found no axis along which they do not overlap, so there is a collision
	return true;
}

////////////////////////////////////////////////////////////////////////////////

vector<vector<int>*>* collision::createGrid(vector<PhysicalObject*>* objects, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes) {
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

////////////////////////////////////////////////////////////////////////////////

void collision::removeFromGrid(vector<vector<int>*>* uniformGrid, PhysicalObject* obj) {
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

////////////////////////////////////////////////////////////////////////////////

void collision::addToGrid(vector<vector<int>*>* uniformGrid, PhysicalObject* obj, glm::vec3 gridMin, glm::vec3 gridMax, glm::vec3 gridSizes) {
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

////////////////////////////////////////////////////////////////////////////////