/**************************************************
SurfaceShader is a shader that has the uniform
 variables for modelview, projection, the
 parameters for the materials, and a list of lights.
*****************************************************/
#include "Shader.h"
#include "Material.h"
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#ifndef __SURFACESHADER_H__
#define __SURFACESHADER_H__

struct SurfaceShader : Shader {
    // modelview and projection
    glm::mat4 view = glm::mat4(1.0f); GLuint view_loc;
    glm::mat4 modelview = glm::mat4(1.0f); GLuint modelview_loc;
    glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;
    // material parameters
    Material* material;
    GLuint ambient_loc;
    GLuint diffuse_loc;
    GLuint specular_loc;
    GLuint emision_loc;
    GLuint shininess_loc;
    
    // lights
    GLboolean enablelighting = GL_FALSE; // are we lighting at all (global).
    GLint nlights = 0;               // number of lights used
    std::vector<glm::vec4> lightpositions; // positions of lights
    std::vector<glm::vec4> lightcolors; // colors of lights
    GLuint enablelighting_loc;
    GLuint nlights_loc;
    GLuint lightpositions_loc;
    GLuint lightcolors_loc;
    
    
    void initUniforms(){
        view_loc  = glGetUniformLocation( program, "view" );
        modelview_loc  = glGetUniformLocation( program, "modelview" );
        projection_loc = glGetUniformLocation( program, "projection" );
        ambient_loc    = glGetUniformLocation( program, "ambient" );
        diffuse_loc    = glGetUniformLocation( program, "diffuse" );
        specular_loc   = glGetUniformLocation( program, "specular" );
        emision_loc    = glGetUniformLocation( program, "emision" );
        shininess_loc  = glGetUniformLocation( program, "shininess" );
        enablelighting_loc = glGetUniformLocation( program, "enablelighting" );
        nlights_loc = glGetUniformLocation( program, "nlights" );
        lightpositions_loc = glGetUniformLocation( program, "lightpositions" );
        lightcolors_loc = glGetUniformLocation( program, "lightcolors" );
        
    }
    void setUniforms(){
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &modelview[0][0]);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);
        glUniform4fv( ambient_loc  , 1, &(material -> ambient[0])  );
        glUniform4fv( diffuse_loc  , 1, &(material -> diffuse[0])  );
        glUniform4fv( specular_loc , 1, &(material -> specular[0]) );
        glUniform4fv( emision_loc  , 1, &(material -> emision[0])  );
        glUniform1fv( shininess_loc, 1, &(material -> shininess)   );
        glUniform1i( enablelighting_loc, enablelighting );
        glUniform1i( nlights_loc, nlights );
        glUniform4fv( lightpositions_loc, GLsizei(nlights), &lightpositions[0][0] );
        glUniform4fv( lightcolors_loc, GLsizei(nlights), &lightcolors[0][0] );
    }
};

#endif 
