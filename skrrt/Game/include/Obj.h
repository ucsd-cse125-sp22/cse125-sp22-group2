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
    GLuint specularID; 
    int width, height, nrChannels; 
public:
    //int object_number;
    void init(const char* filename, const char* texture_filename, const char* specular_filename, int obj_num);
    void init(const char * filename, const char * texture_filename, int obj_num);
    void draw(void);
};

#endif 
