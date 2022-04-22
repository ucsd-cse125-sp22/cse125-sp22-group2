#include "BoundingBox.hpp"

////////////////////////////////////////////////////////////////////////////////

BoundingBox::BoundingBox() {

}

////////////////////////////////////////////////////////////////////////////////

BoundingBox::BoundingBox(int id, glm::vec3 vertex, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	this->id = id;
	corner = vertex;
	aVec = a;
	bVec = b;
	cVec = c;

	vertices.push_back(vertex);
	vertices.push_back(vertex + a);
	vertices.push_back(vertex + b);
	vertices.push_back(vertex + c);
	vertices.push_back(vertex + a + b);
	vertices.push_back(vertex + a + c);
	vertices.push_back(vertex + b + c);
	vertices.push_back(vertex + a + b + c);

	this->computeMinMax();
	bbMin = vertex;
	bbMin = min(bbMin, vertex + a);
	bbMax = vertex;

	gridCells = vector<int>();
}

////////////////////////////////////////////////////////////////////////////////

//BoundingBox::BoundingBox(int id, vector<glm::vec3> vertices)
//{
//	this->id = id;
//	corner = vertex;
//	aVec = a;
//	bVec = b;
//	cVec = c;
//
//	vertices.push_back(vertex);
//	vertices.push_back(vertex + a);
//	vertices.push_back(vertex + b);
//	vertices.push_back(vertex + c);
//	vertices.push_back(vertex + a + b);
//	vertices.push_back(vertex + a + c);
//	vertices.push_back(vertex + b + c);
//	vertices.push_back(vertex + a + b + c);
//
//	this->computeMinMax();
//	bbMin = vertex;
//	bbMin = min(bbMin, vertex + a);
//	bbMax = vertex;
//
//	gridCells = vector<int>();
//}

////////////////////////////////////////////////////////////////////////////////

BoundingBox::~BoundingBox()
{
}

////////////////////////////////////////////////////////////////////////////////

void BoundingBox::computeMinMax()
{
	bbMin = vertices[0];
	bbMax = vertices[0];

	for (unsigned int i = 1; i < vertices.size(); i++) {
		for (unsigned int axis = 0; axis < 3; axis++) {
			bbMin[axis] = min(bbMin[axis], vertices[i][axis]);
			bbMax[axis] = max(bbMax[axis], vertices[i][axis]);
		}
	}

	// Alternate implementation without saving all vertices
	//this->bbMin = this->corner;
	//this->bbMax = this->corner;
	//for (unsigned int axis = 0; axis < 3; axis++) {
	//	bbMin[axis] = min(bbMin[axis], corner[axis] + aVec[axis]);
	//	bbMin[axis] = min(bbMin[axis], corner[axis] + bVec[axis]);
	//	bbMin[axis] = min(bbMin[axis], corner[axis] + cVec[axis]);
	//	bbMin[axis] = min(bbMin[axis], corner[axis] + aVec[axis] + bVec[axis]);
	//	bbMin[axis] = min(bbMin[axis], corner[axis] + aVec[axis] + cVec[axis]);
	//	bbMin[axis] = min(bbMin[axis], corner[axis] + bVec[axis] + cVec[axis]);
	//	bbMin[axis] = min(bbMin[axis], corner[axis] + aVec[axis] + bVec[axis] + cVec[axis]);

	//	bbMax[axis] = max(bbMax[axis], corner[axis] + aVec[axis]);
	//	bbMax[axis] = max(bbMax[axis], corner[axis] + bVec[axis]);
	//	bbMax[axis] = max(bbMax[axis], corner[axis] + cVec[axis]);
	//	bbMax[axis] = max(bbMax[axis], corner[axis] + aVec[axis] + bVec[axis]);
	//	bbMax[axis] = max(bbMax[axis], corner[axis] + aVec[axis] + cVec[axis]);
	//	bbMax[axis] = max(bbMax[axis], corner[axis] + bVec[axis] + cVec[axis]);
	//	bbMax[axis] = max(bbMax[axis], corner[axis] + aVec[axis] + bVec[axis] + cVec[axis]);
	//}
}

////////////////////////////////////////////////////////////////////////////////