/**************************************************
Obj is subclass class of Geometry
that loads an obj file.
*****************************************************/
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "Obj.h"
#include "Debug.h"

void Obj::init(const char* filename, const char* texture_filename, const char* specular_filename, int obj_num){
    init(filename, texture_filename, obj_num);

    // load texture file
    glGenTextures(1, &specularID); 
    glActiveTexture(GL_TEXTURE0 + (1 + object_number*2));
    glBindTexture(GL_TEXTURE_2D, specularID); 

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* image = stbi_load(specular_filename, &width, &height, &nrChannels, 0);

    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image);
}

void Obj::init(const char * filename, const char * texture_filename, int obj_num){

    object_number = obj_num;

    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec2 > uvs;
    std::vector< glm::vec3 > normals;
    std::vector< unsigned int > indices;
    
    // load obj file
    bool loaded = objloader::loadOBJ(filename, vertices, uvs, normals, indices);
    if (!loaded) {
        std::cerr << "Error loading model " << filename << std::endl;
    }

    if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
        std::cout << "done." << std::endl;
    }

    // load texture file
    glGenTextures(1, &textureID); 
    glActiveTexture(GL_TEXTURE0 + (object_number*2));
    glBindTexture(GL_TEXTURE_2D, textureID); 

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* image = stbi_load(texture_filename, &width, &height, &nrChannels, 0);

    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image);

    unsigned int n = indices.size();
    
    // setting up buffers
    if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
        std::cout << "Setting up buffers...";
    }
    glGenVertexArrays(1, &vao );
    buffers.resize(4);
    glGenBuffers(4, buffers.data());
    glBindVertexArray(vao);
    
    // 0th attribute: position
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, n*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    
    // 1st attribute: normal
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, n*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    
    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n*sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,1,GL_INT,GL_FALSE,0,(void*)0);

    // uv's
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, n*sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    /*
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    */

    count = n;
    glBindVertexArray(0);
    if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
        std::cout << "done." << std::endl;
    }

    //glUniform1i(glGetUniformLocation( , "texture" + object_number), 0); 
}

void Obj::draw(void){
    glActiveTexture(GL_TEXTURE0 + (object_number*2));
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE0 + (1+object_number*2));
    glBindTexture(GL_TEXTURE_2D, specularID);

	glBindVertexArray(vao);
	glDrawElements(mode,count,type,0);
}


