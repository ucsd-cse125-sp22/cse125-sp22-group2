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
    geometry["pink_car"] = new Obj;
    geometry["pink_car"]->init("models/CarPink.obj", "textures/car_textures.png", 0);
    geometry["blue_car"] = new Obj;
    geometry["blue_car"]->init("models/CarBlue.obj", "textures/car_textures.png", 0);
    geometry["yellow_car"] = new Obj;
    geometry["yellow_car"]->init("models/CarYellow.obj", "textures/car_textures.png", 0);
    geometry["green_car"] = new Obj;
    geometry["green_car"]->init("models/CarGreen.obj", "textures/car_textures.png", 0);

    geometry["tire"] = new Obj;
    geometry["tire"]->init("models/Tire.obj", "textures/car_textures.png", 1);
    geometry["spotlight"] = new Obj;
    geometry["spotlight"]->init("models/Spotlight.obj", "textures/crown_spotlight_light.png", 2);
    geometry["traffic_light"] = new Obj;
    geometry["traffic_light"]->init("models/TrafficLight.obj", "textures/crown_spotlight_light.png", 2);
    
    // Create a material palette
    material["wood"] = new Material;
    material["wood"] -> ambient = vec4(0.1f,0.1f,0.1f,1.0f);
    material["wood"] -> diffuse = vec4(0.3f,0.15f,0.1f,1.0f);
    material["wood"] -> specular = vec4(0.3f,0.15f,0.1f,1.0f);
    material["wood"] -> shininess = 100.0f;
    
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
    
    // Create a model palette
    /*
    model["car1"] = new Model; 
    model["car1"]->geometry = geometry["car1"]; 
    model["car1"]->material = material["wood"];

    model["car2"] = new Model; 
    model["car2"]->geometry = geometry["car2"]; 
    model["car2"]->material = material["wood"];
    */
    model["pink_car"] = new Model; 
    model["pink_car"]->geometry = geometry["pink_car"]; 
    model["pink_car"]->material = material["silver"];
    model["blue_car"] = new Model; 
    model["blue_car"]->geometry = geometry["blue_car"]; 
    model["blue_car"]->material = material["silver"];
    model["yellow_car"] = new Model; 
    model["yellow_car"]->geometry = geometry["yellow_car"]; 
    model["yellow_car"]->material = material["silver"];
    model["green_car"] = new Model; 
    model["green_car"]->geometry = geometry["green_car"]; 
    model["green_car"]->material = material["silver"];

    model["tire"] = new Model; 
    model["tire"]->geometry = geometry["tire"]; 
    model["tire"]->material = material["ceramic"]; 

    // Create a light palette
    light["sun"] = new Light;
    light["sun"] -> position = vec4(3.0f,2.0f,1.0f,0.0f);
    light["sun"] -> color = 1.0f*vec4(1.0f,1.0f,1.0f,1.0f);
    
    light["bulb"] = new Light;
    light["bulb"] -> position = vec4(0.0f,2.0f,0.0f,0.0f);
    light["bulb"] -> color = 1.5f * vec4(1.0f,0.2f,0.1f,1.0f);
    
    // Build the scene graph
    node["pink_car"] = new Node;
    node["pink_car"]->models.push_back(model["pink_car"]);
    node["pink_car"]->modeltransforms.push_back(mat4(1.0f));

    node["blue_car"] = new Node;
    node["blue_car"]->models.push_back(model["blue_car"]);
    node["blue_car"]->modeltransforms.push_back(mat4(1.0f));

    node["yellow_car"] = new Node;
    node["yellow_car"]->models.push_back(model["yellow_car"]);
    node["yellow_car"]->modeltransforms.push_back(mat4(1.0f));

    node["green_car"] = new Node;
    node["green_car"]->models.push_back(model["green_car"]);
    node["green_car"]->modeltransforms.push_back(mat4(1.0f));

    // Tires
    node["p_tire_f"] = new Node; 
    node["p_tire_f"]->models.push_back(model["tire"]); 
    node["p_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    node["p_tire_b"] = new Node; 
    node["p_tire_b"]->models.push_back(model["tire"]); 
    node["p_tire_b"]->modeltransforms.push_back(mat4(1.0f));
    
    node["b_tire_f"] = new Node; 
    node["b_tire_f"]->models.push_back(model["tire"]); 
    node["b_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    node["b_tire_b"] = new Node; 
    node["b_tire_b"]->models.push_back(model["tire"]); 
    node["b_tire_b"]->modeltransforms.push_back(mat4(1.0f));

    node["y_tire_f"] = new Node; 
    node["y_tire_f"]->models.push_back(model["tire"]); 
    node["y_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    node["y_tire_b"] = new Node; 
    node["y_tire_b"]->models.push_back(model["tire"]); 
    node["y_tire_b"]->modeltransforms.push_back(mat4(1.0f));

    node["g_tire_f"] = new Node; 
    node["g_tire_f"]->models.push_back(model["tire"]); 
    node["g_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    node["g_tire_b"] = new Node; 
    node["g_tire_b"]->models.push_back(model["tire"]); 
    node["g_tire_b"]->modeltransforms.push_back(mat4(1.0f));

    node["world"]->childnodes.push_back(node["pink_car"]);
    node["world"]->childtransforms.push_back(translate(vec3(-3.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate( 90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["pink_car"]->childnodes.push_back(node["p_tire_f"]);
    node["pink_car"] -> childtransforms.push_back( translate(vec3(0.0f, 0.0f, 15.0f)) );
    node["pink_car"]->childnodes.push_back(node["p_tire_b"]);
    node["pink_car"]->childtransforms.push_back(translate(vec3(2.5f, 0.0f, 15.0f)));
    
    node["world"]->childnodes.push_back(node["blue_car"]);
    node["world"]->childtransforms.push_back(translate(vec3(-1.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate( 90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["blue_car"]->childnodes.push_back(node["b_tire_f"]);
    node["blue_car"] -> childtransforms.push_back( translate(vec3(0.0f, 0.0f, 15.0f)) );
    node["blue_car"]->childnodes.push_back(node["b_tire_b"]);
    node["blue_car"]->childtransforms.push_back(translate(vec3(2.5f, 0.0f, 15.0f)));

    node["world"]->childnodes.push_back(node["yellow_car"]);
    node["world"]->childtransforms.push_back(translate(vec3(1.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate( 90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["yellow_car"]->childnodes.push_back(node["y_tire_f"]);
    node["yellow_car"] -> childtransforms.push_back( translate(vec3(0.0f, 0.0f, 15.0f)) );
    node["yellow_car"]->childnodes.push_back(node["y_tire_b"]);
    node["yellow_car"]->childtransforms.push_back(translate(vec3(2.5f, 0.0f, 15.0f)));

    node["world"]->childnodes.push_back(node["green_car"]);
    node["world"]->childtransforms.push_back(translate(vec3(3.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate( 90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["green_car"]->childnodes.push_back(node["g_tire_f"]);
    node["green_car"] -> childtransforms.push_back( translate(vec3(0.0f, 0.0f, 15.0f)) );
    node["green_car"]->childnodes.push_back(node["g_tire_b"]);
    node["green_car"]->childtransforms.push_back(translate(vec3(2.5f, 0.0f, 15.0f)));
    
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
