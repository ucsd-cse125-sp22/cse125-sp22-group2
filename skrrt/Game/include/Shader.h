/***********************
This is a shader consisting of vertex shader and fragment shader.
It has the external file reading and compilation capability.
Example:
Shader myshader;
myshader.read_source( <vtxshader_path>, <fragshader_path >);
myshader.compile();
The resulting shader program is the class member "program."
So, to use the program, simply call
glUseProgram(myshader.program);
The users are welcomed to subclass this Shader class. For example,
class ShaderForMyProject : Shader{
public:
    GLuint some_uniform_variable_position;
    GLfloat some_uniform_variable_value;
}
to further encapsulate various parameters that are associated with
your shader.
 ***********************/

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <cstring>

#ifndef __SHADER_H__
#define __SHADER_H__

class Shader {
private:
    GLuint vertexshader;      // intermediate shader object
    GLuint fragmentshader;    //    before the linker stage
    GLint compiled_vs = 0;   // compile status
    GLint compiled_fs = 0;   // compile status
    GLint linked = 0;        // link status
public:
    GLuint program;  // the shader program
    std::string vertexshader_source;   // source code
    std::string fragmentshader_source; // source code

    void read_source(const char * vertexshader_filename, const char * fragmentshader_filename);
    void compile();
    GLint getVertexShaderCompileStatus(){return compiled_vs;}
    GLint getFragmentShaderCompileStatus(){return compiled_fs;}
    GLint getLinkStatus(){return linked;}
    
private:
    // Helper functions
    static std::string textFileRead(const char * filename );
    static void programerrors(const GLint program);
    static void shadererrors(const GLint shader);
};

#endif 
