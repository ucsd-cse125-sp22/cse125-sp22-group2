#include "BoundingBox.hpp"

using namespace bounding;

////////////////////////////////////////////////////////////////////////////////

BoundingBox::BoundingBox() {
	// This shouldn't ever happen
	BoundingBox(0, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
}

BoundingBox::BoundingBox(int id, glm::vec3 vertex, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	this->id = id;
	this->corner = vertex;
	this->aVec = a;
	this->bVec = b;
	this->cVec = c;

	this->vertices = vector<glm::vec3>();
	this->vertices.push_back(vertex);
	this->vertices.push_back(vertex + a);
	this->vertices.push_back(vertex + b);
	this->vertices.push_back(vertex + c);
	this->vertices.push_back(vertex + a + b);
	this->vertices.push_back(vertex + a + c);
	this->vertices.push_back(vertex + b + c);
	this->vertices.push_back(vertex + a + b + c);

	this->computeMinMax();
	gridCells = vector<int>();
}

BoundingBox::BoundingBox(int id, vector<glm::vec3> vertices)
{
	this->id = id;
	this->vertices = vertices;
	this->corner = vertices[0];
	this->aVec = vertices[1] - vertices[0];
	this->bVec = vertices[2] - vertices[0];
	this->cVec = vertices[3] - vertices[0];

	this->computeMinMax();
	gridCells = vector<int>();
}

bounding::BoundingBox::BoundingBox(int id, glm::vec3 pos, glm::vec3 dir, glm::vec3 up, float l, float w, float h)
{
	this->id = id;
	this->center = pos;

	this->aVec = dir;
	this->bVec = up;
	this->cVec = glm::normalize(glm::cross(dir, up));

	this->corner = pos - (0.5f * l * aVec) - (0.5f * w * bVec) - (0.5f * h * cVec);
	glm::vec3 a = aVec * l;
	glm::vec3 b = bVec * w;
	glm::vec3 c = cVec * h;

	this->vertices = vector<glm::vec3>();
	this->vertices.push_back(corner);
	this->vertices.push_back(corner + a);
	this->vertices.push_back(corner + b);
	this->vertices.push_back(corner + c);
	this->vertices.push_back(corner + a + b);
	this->vertices.push_back(corner + a + c);
	this->vertices.push_back(corner + b + c);
	this->vertices.push_back(corner + a + b + c);

	this->computeMinMax();
	gridCells = vector<int>();
}

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

bool bounding::checkCollision(BoundingBox a, BoundingBox b) {

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