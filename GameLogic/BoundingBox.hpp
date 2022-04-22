#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include "core.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

class BoundingBox
{
private:

public:

	// BoundingBox Information
	int id;
	vector<glm::vec3> vertices;
	glm::vec3 corner;
	glm::vec3 aVec, bVec, cVec;
	glm::vec3 bbMin;
	glm::vec3 bbMax;
	vector<int> gridCells;

	BoundingBox();
	BoundingBox(int id, glm::vec3 vertex, glm::vec3 a, glm::vec3 b, glm::vec3 c);
	~BoundingBox();
	void computeMinMax();
};

////////////////////////////////////////////////////////////////////////////////

#endif