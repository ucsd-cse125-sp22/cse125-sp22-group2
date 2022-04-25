#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include "core.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
namespace bounding {
	class BoundingBox
	{
	private:

	public:
		// Matches the id of the object this bounding box belongs to
		int id;
		// Center of the bounding box
		glm::vec3 center;
		// The eight vertices
		vector<glm::vec3> vertices;
		// The lowest corner of the bounding box, might not be necessary
		glm::vec3 corner;
		// Vectors representing length, width, and height; share the same direction as three of the box's normals
		glm::vec3 aVec, bVec, cVec;

		// Used for the uniform grid, might not need to be saved
		glm::vec3 bbMin;
		glm::vec3 bbMax;

		// What cells this bounding box occupies in the uniform grid
		vector<int> gridCells;

		BoundingBox();
		// Create using a corner and three vectors
		BoundingBox(int id, glm::vec3 vertex, glm::vec3 a, glm::vec3 b, glm::vec3 c);
		// Create using a list of all vertices
		BoundingBox(int id, vector<glm::vec3> vertices);
		// Create using the center, direction, and dimensions
		BoundingBox(int id, glm::vec3 pos, glm::vec3 dir, glm::vec3 up, float l, float w, float h);
		~BoundingBox();
		void computeMinMax();
	};

	// Check whether two bounding boxes overlap
	bool checkCollision(BoundingBox a, BoundingBox b);
}
////////////////////////////////////////////////////////////////////////////////

#endif