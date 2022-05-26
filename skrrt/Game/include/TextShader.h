#include "Shader.h"

#include <vector>
#include <string>
#include "Debug.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#ifndef __TEXTSHADER_H__
#define __TEXTSHADER_H__

struct TextShader : Shader {

	glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;
	glm::vec3 textColor = glm::vec3(1.0f); GLuint textColor_loc; 
	GLuint text = 0; GLuint text_loc;

	void initUniforms() {
		projection_loc = glGetUniformLocation(program, "projection");
		textColor_loc = glGetUniformLocation(program, "textColor");
		text_loc = glGetUniformLocation(program, "text");
	}

	void setUniforms() {
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);
		glUniform3fv(textColor_loc, 1, &(textColor[0])); 
		glUniform1f(text_loc, text);
	}
};

#endif
