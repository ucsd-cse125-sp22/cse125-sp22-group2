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
    float near_plane; GLuint near_plane_loc;
    float far_plane; GLuint far_plane_loc;

    GLuint texture_id;          // indicates which texture to render on object
    GLuint texture_id_loc;
    
    void initUniforms(){
        near_plane_loc  = glGetUniformLocation( program, "near_plane" );
        far_plane_loc = glGetUniformLocation( program, "far_plane" );
        texture_id_loc = glGetUniformLocation(program, "depthMap");
    }
    void setUniforms() {
        glUniform1f(near_plane_loc, near_plane);
        glUniform1f(far_plane_loc, far_plane);
        glUniform1i(texture_id_loc, texture_id);
    }
};

#endif 
