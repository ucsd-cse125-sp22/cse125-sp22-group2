/**************************************************
Obj is subclass class of Geometry
that loads an obj file.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "Geometry.h"
#include "objloader.h"
#include "stb_image.h"
#ifndef __OBJ_H__
#define __OBJ_H__

class Obj : public Geometry {
private: 
    GLuint textureID; 
    int width, height, nrChannels; 
public:

    void init(const char * filename, const char * texture_filename);
    void draw(void);
        
};

#endif 
