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
    GLuint ui_id; GLuint ui_id_loc;
    GLuint drip_id; GLuint drip_id_loc;
    GLuint part_id; GLuint part_id_loc;
    
    void initUniforms(){
        texture_id_loc = glGetUniformLocation(program, "depthMap");
        bloom_id_loc = glGetUniformLocation(program, "bloom");
        ui_id_loc = glGetUniformLocation(program, "uiTex");
        drip_id_loc = glGetUniformLocation(program, "dripTex");
        part_id_loc = glGetUniformLocation(program, "partTex");
        exposure_loc = glGetUniformLocation(program, "exposure");
    }
    void setUniforms() {
        glUniform1i(texture_id_loc, texture_id);
        glUniform1i(bloom_id_loc, bloom_id);
        glUniform1i(ui_id_loc, ui_id);
        glUniform1i(drip_id_loc, drip_id);
        glUniform1i(part_id_loc, part_id);
        glUniform1f(exposure_loc, exposure);
    }
};

#endif 
