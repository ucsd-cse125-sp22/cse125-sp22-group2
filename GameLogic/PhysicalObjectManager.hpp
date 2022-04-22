#pragma once

#include "core.h"
#include "BoundingBox.hpp"
#include "Collision.hpp"

class PhysicalObjectManager
{
private:

public:
	vector<PhysicalObject*>* objects;
	vector<vector<int>*>* uniformGrid;
	// vector<unsigned int> vacancies;
};