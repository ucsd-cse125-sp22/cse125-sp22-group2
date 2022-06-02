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
    GLuint bloom_id;
    GLuint bloom_id_loc;
    float exposure;
    GLuint exposure_loc;
    
    void initUniforms(){
        texture_id_loc = glGetUniformLocation(program, "depthMap");
        bloom_id_loc = glGetUniformLocation(program, "bloom");
        exposure_loc = glGetUniformLocation(program, "exposure");
    }
    void setUniforms() {
        glUniform1i(texture_id_loc, texture_id);
        glUniform1i(bloom_id_loc, bloom_id);
        glUniform1f(exposure_loc, exposure);
    }
};

#endif 
