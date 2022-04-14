/**************************************************

*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

struct Material {
    glm::vec4 ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 emision = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float shininess = 10.0f;
};

#endif 
