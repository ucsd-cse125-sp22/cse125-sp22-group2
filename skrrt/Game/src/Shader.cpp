#include <string>

#include "Shader.h"

using namespace std ;

void Shader::read_source(const char * vertexshader_filename, const char * fragmentshader_filename)  {
    vertexshader_source = textFileRead(vertexshader_filename);
    fragmentshader_source = textFileRead(fragmentshader_filename);
}
void Shader::compile()  {
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar * cstr_vs = vertexshader_source.c_str() ; // convert source to const GLchar *
    const GLchar * cstr_fs = fragmentshader_source.c_str() ;
    glShaderSource( vertexshader, 1, &cstr_vs, NULL) ;
    glShaderSource( fragmentshader, 1, &cstr_fs, NULL) ;
    glCompileShader( vertexshader );
    glCompileShader( fragmentshader );
    glGetShaderiv ( vertexshader, GL_COMPILE_STATUS, &compiled_vs ) ;
    glGetShaderiv ( fragmentshader, GL_COMPILE_STATUS, &compiled_fs ) ;
    if (!compiled_vs) {
        cout << "Vertex Shader ";
        shadererrors( vertexshader ) ;
        throw 3 ;
    }
    if (!compiled_fs) {
        cout << "Fragment Shader ";
        shadererrors( fragmentshader ) ;
        throw 3 ;
    }
    program = glCreateProgram() ;
    glAttachShader(program, vertexshader) ;
    glAttachShader(program, fragmentshader) ;
    glLinkProgram(program) ;
    glGetProgramiv(program, GL_LINK_STATUS, &linked) ;
    if (linked){
        glDetachShader( program, vertexshader );
        glDetachShader( program, fragmentshader );
        glDeleteShader( vertexshader );
        glDeleteShader( fragmentshader );
    }else{
        programerrors(program) ;
        throw 4 ;
    }
}

// Below are helper functions

string Shader::textFileRead (const char * filename) {
	string str, ret = "" ; 
	ifstream in ; 
	in.open(filename) ; 
	if (in.is_open()) {
		getline (in, str) ; 
		while (in) {
			ret += str + "\n" ; 
			getline (in, str) ; 
		}
		//    cout << "Shader below\n" << ret << "\n" ; 
		return ret ; 
	}
	else {
		cerr << "Unable to Open File " << filename << "\n" ; 
		throw 2 ; 
	}
}

void Shader::programerrors (const GLint program) {
	GLint length ; 
	GLchar * log ; 
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length) ; 
	log = new GLchar[length+1] ;
	glGetProgramInfoLog(program, length, &length, log) ; 
	cout << "Compile Error, Log Below\n" << log << "\n" ; 
	delete [] log ; 
}
void Shader::shadererrors (const GLint shader) {
	GLint length ; 
	GLchar * log ; 
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length) ; 
	log = new GLchar[length+1] ;
	glGetShaderInfoLog(shader, length, &length, log) ; 
	cout << "Compile Error, Log Below\n" << log << "\n" ; 
	delete [] log ; 
}
