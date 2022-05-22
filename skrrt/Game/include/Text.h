#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>
#include <GL/glew.h>

#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <utility>

#include <ft2build.h>
#include FT_FREETYPE_H  

#include "../../../Definitions.hpp"

#ifndef __TEXT_H__
#define __TEXT_H__

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

class Text {
private:

	GLuint shader; 
	std::map<char, Character> Characters;
	std::string text;

	float x, y; 
	float scale; 
	glm::vec3 color; 

	unsigned int VAO, VBO;

public:

	Text(GLuint s) {
		shader = s;

		// pre load all 128 ascii characters
		init();

		x = 0; 
		y = 0; 
		scale = 30; 
		color = glm::vec3(1.0f);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);      
	}

	void init();

	//void setShader(GLuint s) { shader = s; };

	void updateText(std::string t) { text = t; }
	void setPosition(float a, float b) { x = a; y = b; };
	void setScale(float s) { scale = s; };
	void setColor(glm::vec3 c) { color = c; };

	glm::vec3 getColor() { return color;  }

	void RenderText(void);
};

#endif
