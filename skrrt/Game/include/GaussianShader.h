/**************************************************
SurfaceShader is a shader that has the uniform
 variables for modelview, projection, the
 parameters for the materials, and a list of lights.
*****************************************************/
#include "Shader.h"
#include "ShadowMapConstants.h"
#include "Debug.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#ifndef __GAUSSIAN_SHADER_H__
#define __GAUSSIAN_SHADER_H__

struct GaussianShader : Shader {
    // modelview and projection

    GLuint image; GLuint image_loc;
    GLuint horizontal; GLuint horizontal_loc;
    
    void initUniforms(){
        image_loc = glGetUniformLocation(program, "image");
        horizontal_loc = glGetUniformLocation(program, "horizontal");
    }
    void setUniforms() {
        glUniform1i(image_loc, image);
        glUniform1i(horizontal_loc, horizontal);
    }
};

#endif 
