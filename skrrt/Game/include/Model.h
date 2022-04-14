/**************************************************
A model is a geometry with material.
*****************************************************/
#include "Geometry.h"
#include "Material.h"

#ifndef __MODEL_H__
#define __MODEL_H__

struct Model {
    Geometry* geometry;
    Material* material;
};

#endif 
