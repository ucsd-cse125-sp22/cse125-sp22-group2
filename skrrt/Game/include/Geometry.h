/**************************************************
Geometry is an abstract class for a geometric object.
 A derived class of Geometry will have an implemention of
   ```void init(void)```
 or
   ```void init(const char* s).```
 The option for const char* input is for a derived class
 that requires reading external files.
 The implementation of the `init` is for initializing the
 geometry (i.e. setting up the buffers etc).
 
 The draw command is fixed.  We can call
 
 glBindVertexArray(obj.vao);
 glDrawElements(obj.mode, obj.count, obj.type, 0);
 
which should explain the purpose of those class members.
 We can also just call the "draw()" member function, which
 is equivalent to the commands above.
 
The array of buffers is encapsulated in std::vector so
we do not need to manually allocate/free the memory for
arrays of unknown size.
*****************************************************/
#include <vector>
#include <SurfaceShader.h>
#include "DepthShader.h"

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

class Geometry {
public:
    GLenum mode = GL_TRIANGLES; // the cookboook for glDrawElements
    int count; // number of elements to draw
    GLenum type = GL_UNSIGNED_INT; // type of the index array
    GLuint vao; // vertex array object a.k.a. geometry spreadsheet
    std::vector<GLuint> buffers; // data storage

    int object_number = 0;
    
    virtual void init(){};
    virtual void init(const char* s){};
    virtual void init(const char* s, const char* t){};
    virtual void init(const char* s, const char* t, int obj_num){};
    virtual void init(const char* s, const char* t, const char* spec, const char* emis, int obj_num){};
    
    void draw(SurfaceShader *shader){
        glBindVertexArray(vao);
        shader->setUniforms();
		glCullFace(GL_BACK); 
	    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glDrawElements(mode,count,type,0);
    }

    void drawDepth(DepthShader* shader){
        glBindVertexArray(vao);
        shader->setUniforms();
		glCullFace(GL_BACK); 
	    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glDrawElements(mode,count,type,0);
    }
};

#endif 
