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
#ifndef __DEPTH_SHADER_H__
#define __DEPTH_SHADER_H__

struct DepthShader : Shader {
    // modelview and projection
    glm::mat4 model = glm::mat4(1.0f); GLuint model_loc;
    glm::mat4 lightSpace = glm::mat4(1.0f); GLuint lightSpace_loc;
    
    void initUniforms(){
        model_loc  = glGetUniformLocation( program, "model" );
        lightSpace_loc = glGetUniformLocation( program, "lightSpace" );
    }
    void setUniforms() {
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(lightSpace_loc, 1, GL_FALSE, &lightSpace[0][0]);
    }
};

#endif 
