/**************************************************
Light is a class for a camera object.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#ifndef __LIGHT_H__
#define __LIGHT_H__

struct DirectionalLight {
    // The direction vector the light shines in
    glm::vec3 direction;

    //How the light looks (color, shininess, ...)
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

struct PointLight {
    // Position of the light in world space
    glm::vec4 position;

    // Attenuation (How the light fades with distance)
    // for no attenuation, set to (1.0f, 0.0f, 0.0f)
    float constant = 1.0f;
    float linear = 0.0f;
    float quadradic = 0.0f;

    // How the light looks (color, shininess, ...)
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

#endif 
