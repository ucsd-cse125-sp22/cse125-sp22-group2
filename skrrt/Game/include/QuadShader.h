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
#ifndef __QUAD_SHADER_H__
#define __QUAD_SHADER_H__

struct QuadShader : Shader {
    // modelview and projection

    GLuint texture_id;          // indicates which texture to render on object
    GLuint texture_id_loc;
    
    void initUniforms(){
        texture_id_loc = glGetUniformLocation(program, "depthMap");
    }
    void setUniforms() {
        glUniform1i(texture_id_loc, texture_id);
    }
};

#endif 
