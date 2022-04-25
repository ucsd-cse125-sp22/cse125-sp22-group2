/**************************************************
Scene.inl contains the definition of the scene graph
*****************************************************/
#include "Scene.h"
#include "Cube.h"
#include "Obj.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace glm;
void Scene::init(void){
    // Create a geometry palette
    geometry["car1"] = new Obj;
    geometry["car1"]->init("models/Car.obj", "textures/pink_car.png", 0);
    geometry["car2"] = new Obj;
    geometry["car2"]->init("models/Car.obj", "textures/fake_blue_car.png", 1);
    
    // Create a material palette
    material["wood"] = new Material;
    material["wood"] -> ambient = vec4(0.1f,0.1f,0.1f,1.0f);
    material["wood"] -> diffuse = vec4(0.3f,0.15f,0.1f,1.0f);
    material["wood"] -> specular = vec4(0.3f,0.15f,0.1f,1.0f);
    material["wood"] -> shininess = 100.0f;
    
    /*
    material["ceramic"] = new Material;
    material["ceramic"] -> ambient = vec4(0.02f, 0.07f, 0.2f, 1.0f);
    material["ceramic"] -> diffuse = vec4(0.1f, 0.25f, 0.7f, 1.0f);
    material["ceramic"] -> specular = vec4(0.9f, 0.9f, 0.9f, 1.0f);
    material["ceramic"] -> shininess = 150.0f;
 
    material["silver"] = new Material;
    material["silver"] -> ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
    material["silver"] -> diffuse = vec4(0.2f, 0.2f, 0.2f, 1.0f);
    material["silver"] -> specular = vec4(0.9f, 0.9f, 0.9f, 1.0f);
    material["silver"] -> shininess = 50.0f;
    
    material["turquoise"] = new Material;
    material["turquoise"] -> ambient = vec4(0.1f, 0.2f, 0.17f, 1.0f);
    material["turquoise"] -> diffuse = vec4(0.2f, 0.375f, 0.35f, 1.0f);
    material["turquoise"] -> specular = vec4(0.3f, 0.3f, 0.3f, 1.0f);
    material["turquoise"] -> shininess = 100.0f;
    
    material["bulb"] = new Material;
    material["bulb"] -> ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    material["bulb"] -> diffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    material["bulb"] -> specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    material["bulb"] -> emision = vec4(1.0f,0.2f,0.1f,1.0f);
    material["bulb"] -> shininess = 200.0f;
    */
    
    // Create a model palette
    model["car1"] = new Model; 
    model["car1"]->geometry = geometry["car1"]; 
    model["car1"]->material = material["wood"];

    model["car2"] = new Model; 
    model["car2"]->geometry = geometry["car2"]; 
    model["car2"]->material = material["wood"];
    
    // Create a light palette
    light["sun"] = new Light;
    light["sun"] -> position = vec4(3.0f,2.0f,1.0f,0.0f);
    light["sun"] -> color = 1.0f*vec4(1.0f,1.0f,1.0f,1.0f);
    
    light["bulb"] = new Light;
    light["bulb"] -> position = vec4(0.0f,2.0f,0.0f,0.0f);
    light["bulb"] -> color = 1.5f * vec4(1.0f,0.2f,0.1f,1.0f);
    
    // Build the scene graph
    node["car1"] = new Node;
    node["car1"]->models.push_back(model["car1"]);
    node["car1"]->modeltransforms.push_back(translate(vec3(1.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate( 90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));

    node["car2"] = new Node;
    node["car2"]->models.push_back(model["car2"]);
    node["car2"]->modeltransforms.push_back(translate(vec3(-1.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate( 90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    
    node["world"]->childnodes.push_back(node["car1"]);
    node["world"] -> childtransforms.push_back( mat4(1.0f) );

    node["world"]->childnodes.push_back(node["car2"]);
    node["world"] -> childtransforms.push_back( mat4(1.0f) );
    
    // Put a camera
    camera = new Camera;
    camera -> target_default = vec3( 0.0f, 1.0f, 0.0f );
    camera -> eye_default = vec3( 0.0f, 1.0f, 5.0f );
    camera -> up_default = vec3( 0.0f, 1.0f, 0.0f );
    camera -> reset(); 
    
    // Initialize shader
    shader = new SurfaceShader;
    shader -> read_source( "shaders/projective.vert", "shaders/lighting.frag" );
    shader -> compile();
    glUseProgram(shader -> program);
    shader -> initUniforms();
}
