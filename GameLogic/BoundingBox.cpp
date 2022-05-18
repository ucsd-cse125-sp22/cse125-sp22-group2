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

	this->corner = pos - (0.5f * l * aVec) - (0.5f * h * bVec) - (0.5f * w * cVec);
	glm::vec3 a = aVec * l;
	glm::vec3 b = bVec * h;
	glm::vec3 c = cVec * w;

	this->vertices = vector<glm::vec3>();
	this->vertices.push_back(corner);
	this->vertices.push_back(corner + a);
	this->vertices.push_back(corner + b);
	this->vertices.push_back(corner + c);
	this->vertices.push_back(corner + a + b);
	this->vertices.push_back(corner + a + c);
	this->vertices.push_back(corner + b + c);
	this->vertices.push_back(corner + a + b + c);

	//for (int i = 0; i < 8; i++) {
	//	cout << "(" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ")\n";
	//}

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
		float a_min = 0.0f;
		float a_max = 0.0f;
		float b_min = 0.0f;
		float b_max = 0.0f;
		// cout << "!NEW AXIS!\n";

		// Get the min/max for a
		for (unsigned int j = 0; j < a.vertices.size(); j++) {
			// Project onto the normal
			float proj = glm::dot(axes[i], a.vertices[j]);
			// cout << "Bounding box a vertex " << j << "(" << a.vertices[j].x << ", " << a.vertices[j].y << ", " << a.vertices[j].z << ") projection: " << proj << "\n";
			// Set min
			if (j == 0 || proj < a_min) {
				a_min = proj;
			}
			// Set max
			if (j == 0 || proj > a_max) {
				a_max = proj;
			}
		}

		// Get the min/max for b
		for (unsigned int j = 0; j < b.vertices.size(); j++) {
			// Project onto the normal
			float proj = glm::dot(axes[i], b.vertices[j]);
			// cout << "Bounding box b vertex " << j << "(" << b.vertices[j].x << ", " << b.vertices[j].y << ", " << b.vertices[j].z << ") projection: " << proj << "\n";
			// Set min
			if (j == 0 || proj < b_min) {
				b_min = proj;
			}
			// Set max
			if (j == 0 || proj > b_max) {
				b_max = proj;
			}
		}

		// Check for a gap
		if (a_max < b_min || a_min > b_max) {
			// Found an axis where they don't overlap, so no collision
			// cout << "DONE! Max a: " << a_max << " b: " << b_max << " Min a: " << a_min << " b: " << b_min << "\n";
			return false;
		}
	}

	// We found no axis along which they do not overlap, so there is a collision
	return true;
}

glm::vec3 bounding::checkCollisionAdjust(BoundingBox a, BoundingBox b)
{
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

	float min_overlap = 9999.0f;
	glm::vec3 min_axis = glm::vec3(0.0f);
	
	// Go through each of the axes we found above
	for (unsigned int i = 0; i < axes.size(); i++) {
		float a_min = 0.0f;
		float a_max = 0.0f;
		float b_min = 0.0f;
		float b_max = 0.0f;
		// cout << "!NEW AXIS!\n";

		// Get the min/max for a
		for (unsigned int j = 0; j < a.vertices.size(); j++) {
			// Project onto the normal
			float proj = glm::dot(glm::normalize(axes[i]), a.vertices[j]);
			// cout << "Bounding box a vertex " << j << "(" << a.vertices[j].x << ", " << a.vertices[j].y << ", " << a.vertices[j].z << ") projection: " << proj << "\n";
			// Set min
			if (j == 0 || proj < a_min) {
				a_min = proj;
			}
			// Set max
			if (j == 0 || proj > a_max) {
				a_max = proj;
			}
		}

		// Get the min/max for b
		for (unsigned int j = 0; j < b.vertices.size(); j++) {
			// Project onto the normal
			float proj = glm::dot(glm::normalize(axes[i]), b.vertices[j]);
			// cout << "Bounding box b vertex " << j << "(" << b.vertices[j].x << ", " << b.vertices[j].y << ", " << b.vertices[j].z << ") projection: " << proj << "\n";
			// Set min
			if (j == 0 || proj < b_min) {
				b_min = proj;
			}
			// Set max
			if (j == 0 || proj > b_max) {
				b_max = proj;
			}
		}

		// Check for a gap
		if (a_max < b_min || a_min > b_max) {
			// Found an axis where they don't overlap, so no collision
			// cout << "DONE! Max a: " << a_max << " b: " << b_max << " Min a: " << a_min << " b: " << b_min << "\n";
			//cout << "Potential adjustment: ???\n";
			return glm::vec3(0.0f);
		}
		else {
			float overlap = 0.0f;
			if (a_max > b_max) {
				overlap = max(b_max - a_min + 0.0001f, 0.0001f);
				//if (overlap <= min_overlap || !i) {
				//	min_overlap = overlap;
				//	min_axis = -glm::normalize(axes[i]);
				//}
			}
			else {
				overlap = max(a_max - b_min + 0.0001f, 0.0001f);
			}
			if (overlap <= min_overlap || !i) {
				min_overlap = overlap;
				min_axis = glm::normalize(axes[i]);
			}
		}
	}

	// We found no axis along which they do not overlap, so there is a collision
	//cout << "Potential adjustment: " << min_axis.x << ", " << min_axis.y << ", " << min_axis.z << ": " << min_overlap << "\n";
	return min_overlap * min_axis;
}

glm::vec3 bounding::checkCollisionRadius(BoundingBox a, glm::vec3 center, float r)
{
	glm::vec3 dir = glm::normalize(glm::vec3(a.center.x, 0.0f, a.center.z) - glm::vec3(center.x, 0.0f, center.z));
	float max_dist = -1.0f;
	for (unsigned int i = 0; i < a.vertices.size(); i++) {
		float curr_dist = glm::dot(glm::vec3(a.vertices[i].x, 0.0f, a.vertices[i].z), dir);
		max_dist = max(max_dist, curr_dist);
	}
	if (max_dist > r) {
		return (r - max_dist) * dir;
	}
	return glm::vec3(0.0f);
}

glm::vec3 bounding::checkCollisionPointFloor(glm::vec3 v, BoundingBox floor)
{
	glm::vec3 dir = glm::normalize(floor.bVec);
	//float f_min = 0.0f;
	float f_max = 0.0f;
	for (unsigned int i = 0; i < floor.vertices.size(); i++) {
		float proj = glm::dot(dir, floor.vertices[i]);
		//if (!i || proj < f_min) {
		//	f_min = proj;
		//}
		if (!i || proj > f_max) {
			f_max = proj;
		}
	}
	float proj = glm::dot(dir, v);
	if (proj <= f_max) {
		return (f_max - proj + 0.0001f) * dir;
	}
	return glm::vec3(0.0f);
}

BoundingBox bounding::checkCollisionFloor(BoundingBox obj, BoundingBox floor, float maxOffset)
{
	// Since previous attempts kind of went nowhere this is now blank
	// This is my plan for slopes though
	// 
	// Part I: Offset all lower vertices to be above the floor
	// Part II: Use the diagonals to get the up vector
	// Part III: Construct a new bounding box
	// Part IV: Adjust the bounding box again to not collide with the floor
	return BoundingBox();
}

BoundingBox bounding::matchTerrain(BoundingBox object, BoundingBox floor, float maxOffset)
{
	return BoundingBox();
}