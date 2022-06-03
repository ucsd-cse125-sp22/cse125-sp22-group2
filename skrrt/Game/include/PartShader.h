/**************************************************
SurfaceShader is a shader that has the uniform
 variables for modelview, projection, the
 parameters for the materials, and a list of lights.
*****************************************************/
#include "Shader.h"
#include "Material.h"
#include "ShadowMapConstants.h"
#include "Light.h"
#include <vector>
#include <string>
#include "Debug.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#ifndef __PARTSHADER_H__
#define __PARTSHADER_H__

struct PartShader : Shader {
    // modelview and projection
    glm::mat4 modelview = glm::mat4(1.0f); GLuint modelview_loc;
    glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;


    glm::vec4 particleColor; GLuint particleColor_loc;
    
    void initUniforms(){
        particleColor_loc = glGetUniformLocation(program, "particleColor"); 

        modelview_loc  = glGetUniformLocation( program, "modelview" );
        projection_loc = glGetUniformLocation( program, "projection" );
    }
    void setUniforms() {
        glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &modelview[0][0]);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);

        glUniform4fv(particleColor_loc, 1, &(particleColor[0]));
    }
};

#endif 
