/**************************************************
SurfaceShader is a shader that has the uniform
 variables for modelview, projection, the
 parameters for the materials, and a list of lights.
*****************************************************/
#include "Shader.h"
#include "Material.h"
#include "Light.h"
#include <vector>
#include "Debug.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#ifndef __SURFACESHADER_H__
#define __SURFACESHADER_H__

struct Material_Locs {
    GLuint specular;
    GLuint emission;
    GLuint shininess;
};

struct PointLight_Locs {
    GLuint position;

    GLuint constant;
    GLuint linear;
    GLuint quadradic;

    GLuint diffuse;
    GLuint ambient;
    GLuint specular;
    GLuint shininess;
};

struct DirectionalLight_Locs {
    GLuint direction;
    GLuint diffuse;
    GLuint ambient;
    GLuint specular;
    GLuint shininess;
};

#define MAX_NUM_POINT_LIGHTS 10

struct SurfaceShader : Shader {
    // modelview and projection
    glm::mat4 view = glm::mat4(1.0f); GLuint view_loc;
    glm::mat4 modelview = glm::mat4(1.0f); GLuint modelview_loc;
    glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;
    // material parameters
    Material* material;
    Material_Locs material_loc;
    
    // lights
    GLboolean enablelighting = GL_FALSE; // are we lighting at all (global).
    GLint numPointLights = 0;               // number of lights used
    std::vector<PointLight*> pointLights; 
    GLuint enablelighting_loc;
    GLuint numPointLights_loc;
    std::vector<PointLight_Locs> pointLights_loc = std::vector<PointLight_Locs>(MAX_NUM_POINT_LIGHTS);
    DirectionalLight* sun;
    DirectionalLight_Locs sun_loc;

    // Texture id 
    GLuint texture_id;          // indicates which texture to render on object
    GLuint texture_id_loc;
    
    GLuint specular_id;
    GLuint specular_id_loc;
    
    void initUniforms(){
        view_loc  = glGetUniformLocation( program, "view" );
        modelview_loc  = glGetUniformLocation( program, "modelview" );
        projection_loc = glGetUniformLocation( program, "projection" );
        material_loc.emission    = glGetUniformLocation( program, "material.emission" );
        material_loc.specular    = glGetUniformLocation( program, "material.specular" );
        material_loc.shininess    = glGetUniformLocation( program, "material.shininess" );
        enablelighting_loc = glGetUniformLocation( program, "enablelighting" );
        numPointLights_loc = glGetUniformLocation( program, "numPointLights" );
        
        for (int i = 0; i < MAX_NUM_POINT_LIGHTS; i++) {
            pointLights_loc[i].position = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].position").c_str());
            pointLights_loc[i].constant = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].constant").c_str());
            pointLights_loc[i].linear = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].linear").c_str());
            pointLights_loc[i].quadradic = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].quadradic").c_str());
            pointLights_loc[i].diffuse = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].diffuse").c_str());
            pointLights_loc[i].ambient = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].ambient").c_str());
            pointLights_loc[i].specular = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].specular").c_str());
            pointLights_loc[i].shininess = glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].shininess").c_str());
            if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
                std::cout << "pointLights_loc[" << i << "].position: " << pointLights_loc[i].position << "\n";
                std::cout << "pointLights_loc[" << i << "].constant: " << pointLights_loc[i].constant << "\n";
                std::cout << "pointLights_loc[" << i << "].specular: " << pointLights_loc[i].specular << "\n";
            }
        }
        
        
        sun_loc.direction = glGetUniformLocation(program, "sun.direction");
        sun_loc.diffuse = glGetUniformLocation(program, "sun.diffuse");
        sun_loc.ambient = glGetUniformLocation(program, "sun.ambient");
        sun_loc.specular = glGetUniformLocation(program, "sun.specular");
        sun_loc.shininess = glGetUniformLocation(program, "sun.shininess");

        texture_id_loc = glGetUniformLocation( program, "texture_id" );
        specular_id_loc = glGetUniformLocation(program, "specular_id");
    }
    void setUniforms(){
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &modelview[0][0]);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);
       
        // set material
        glUniform4fv( material_loc.emission ,1, &(material->emision[0]));
        glUniform4fv( material_loc.specular ,1, &(material->specular[0]));
        glUniform1f( material_loc.shininess,  material->shininess);

        glUniform1i( enablelighting_loc, enablelighting );

        //set point lights
        for (int i = 0; i < numPointLights; i++) {
            glUniform4fv( pointLights_loc[i].position, 1, &(pointLights[i]->position[0]));
            glUniform4fv( pointLights_loc[i].ambient, 1, &(pointLights[i]->ambient[0]));
            glUniform4fv( pointLights_loc[i].diffuse, 1, &(pointLights[i]->diffuse[0]));
            glUniform4fv( pointLights_loc[i].specular, 1, &(pointLights[i]->specular[0]));
            glUniform1f( pointLights_loc[i].constant, pointLights[i]->constant);
            glUniform1f( pointLights_loc[i].linear, pointLights[i]->linear);
            glUniform1f( pointLights_loc[i].quadradic, pointLights[i]->quadradic);
        }
        glUniform1i( numPointLights_loc, numPointLights );
        
        //set sun
        glUniform3fv( sun_loc.direction, 1, &(sun->direction[0]));
        glUniform4fv( sun_loc.diffuse, 1, &(sun->diffuse[0]));
        glUniform4fv( sun_loc.ambient, 1, &(sun->ambient[0]));
        glUniform4fv( sun_loc.specular, 1, &(sun->specular[0]));

        glUniform1i(texture_id_loc, texture_id);
        glUniform1i(specular_id_loc, specular_id);
    }
};

#endif 
