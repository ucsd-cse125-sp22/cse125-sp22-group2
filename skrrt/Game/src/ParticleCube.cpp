#include "ParticleCube.h"


////////////////////////////////////////////////////////////////////////////////

ParticleCube::ParticleCube(glm::vec3 cubeMin, glm::vec3 cubeMax) 
{
	// Model matrix.
	model = glm::mat4(1.0f);

	// The color of the cube. Try setting it to something else!
	//color = glm::vec3(1.0f, 0.95f, 0.1f); 
	color = glm::vec3(0.2f, 0.2f, 0.2f); 

	// Specify vertex positions
	positions = {
		// Front
		glm::vec3(cubeMin.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMax.z),

		// Back
		glm::vec3(cubeMax.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMin.z),

		// Top
		glm::vec3(cubeMin.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMin.z),

		// Bottom
		glm::vec3(cubeMin.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMin.y,cubeMax.z),

		// Left
		glm::vec3(cubeMin.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMin.z),

		// Right
		glm::vec3(cubeMax.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMax.z)
	};

	// Specify normals
	normals = {
		// Front
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),

		// Back			
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),

		// Top
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),

		// Bottom
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),

		// Left
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),

		// Right
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0)
	};
	
	// Specify indices
	indices = {
		0,1,2,		0,2,3,			// Front
		4,5,6,		4,6,7,			// Back
		8,9,10,		8,10,11,		// Top
		12,13,14,	12,14,15,		// Bottom
		16,17,18,	16,18,19,		// Left
		20,21,22,	20,22,23,		// Right
	};


	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);
	glGenBuffers(1, &EBO);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,1,GL_INT,GL_FALSE,0,(void*)0);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////

ParticleCube::~ParticleCube()
{
	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

////////////////////////////////////////////////////////////////////////////////

void ParticleCube::draw(const glm::mat4& viewProjMtx, GLuint shader)
//void ParticleCube::draw(const glm::mat4& viewProjMtx, SurfaceShader* shader) 
{
	// actiavte the shader program 
	glUseProgram(shader);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_CULL_FACE); 
	//glCullFace(GL_FRONT); 

	// get the locations and send the uniforms to the shader 

	//**********
	//glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
	/*
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelview"), 1, GL_FALSE, (float*)&model);
	glUniform1i( glGetUniformLocation(shader, "enablelighting"), 0);

	glUniform1i(glGetUniformLocation(shader, "is_particle"), 1);
	*/

	//************

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Unbind the VAO and shader program
	//glBindVertexArray(0);
	//glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////

void ParticleCube::update()
{
	// Spin the cube by 1 degree.
	spin(0.1f);
}

////////////////////////////////////////////////////////////////////////////////

void ParticleCube::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = model * glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}
