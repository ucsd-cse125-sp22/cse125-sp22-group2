#ifndef __UISHADER_H__
#define __UISHADER_H__

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Debug.h"
#include "Shader.h"

struct UIShader : Shader {

    glm::mat4 modelview = glm::mat4(1.0f); GLuint modelview_loc;
    glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;

    // Texture id 
    GLuint texture_id;          // indicates which texture to render on object
    GLuint texture_id_loc;

	void initUniforms() {
        modelview_loc  = glGetUniformLocation( program, "modelview" );
        projection_loc = glGetUniformLocation( program, "projection" );
        texture_id_loc = glGetUniformLocation( program, "texture_id" );
	}

	void setUniforms() {
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &modelview[0][0]);
        glUniform1i(texture_id_loc, texture_id);
	}
};

#endif
