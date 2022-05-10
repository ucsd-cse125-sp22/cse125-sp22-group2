/**************************************************
Scene.inl contains the definition of the scene graph
*****************************************************/
#include "Scene.h"
#include "Cube.h"
#include "Obj.h"
#define _USE_MATH_DEFINES
#include <math.h>
//#include "..\..\..\Constants.hpp"

#define NUM_PLAYERS 4

using namespace glm;
void Scene::init(void){


    // Create a geometry palette
    geometry["pink_car"] = new Obj;
    geometry["pink_car"]->init("models/Car_Pink.obj", "textures/car_textures.png", 0);
    //geometry["pink_car"]->init("models/Car_Pink.obj", "textures/fake_blue_car.png", 3);
    geometry["blue_car"] = new Obj;
    geometry["blue_car"]->init("models/Car_Blue.obj", "textures/car_textures.png", 0);
    //geometry["blue_car"]->init("models/Car_Blue.obj", "textures/fake_blue_car.png", 3);
    geometry["yellow_car"] = new Obj;
    geometry["yellow_car"]->init("models/Car_Yellow.obj", "textures/car_textures.png", 0);
    //geometry["yellow_car"]->init("models/Car_Yellow.obj", "textures/fake_blue_car.png", 3);
    geometry["green_car"] = new Obj;
    geometry["green_car"]->init("models/Car_Green.obj", "textures/car_textures.png", 0);
    //geometry["green_car"]->init("models/Car_Green.obj", "textures/fake_blue_car.png", 3);

    geometry["tire"] = new Obj;
    geometry["tire"]->init("models/Tires.obj", "textures/car_textures.png", 0);
    geometry["crown"] = new Obj; 
    geometry["crown"]->init("models/Crown.obj", "textures/crown_spotlight_light.png", 1);

    geometry["traffic_light"] = new Obj;
    geometry["traffic_light"]->init("models/TrafficLight.obj", "textures/crown_spotlight_light.png", 1);
    geometry["spotlight"] = new Obj;
    geometry["spotlight"]->init("models/Spotlight.obj", "textures/crown_spotlight_light.png", 1);

    geometry["map"] = new Obj; 
    geometry["map"]->init("models/Map(not finished).obj", "textures/grey.png", 2);

    //geometry["cube"] = new Cube;
    //geometry["cube"]->init();
    geometry["cube"] = new Obj;
    geometry["cube"]->init("models/Spotlight.obj", "texture/fake_blue_car.png", 3);
    
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
    model["tire"]->material = material["silver"]; 

    model["crown"] = new Model; 
    model["crown"]->geometry = geometry["crown"];
    model["crown"]->material = material["silver"]; 

    model["map"] = new Model; 
    model["map"]->geometry = geometry["map"];
    model["map"]->material = material["silver"];

    model["cube"] = new Model; 
    model["cube"]->geometry = geometry["cube"];
    model["cube"]->material = material["silver"];

    // Create a light palette
    light["sun"] = new Light;
    light["sun"] -> position = vec4(3.0f,2.0f,1.0f,0.0f);
    light["sun"] -> color = 1.0f*vec4(1.0f,1.0f,1.0f,1.0f);
    
    light["bulb"] = new Light;
    light["bulb"] -> position = vec4(0.0f,2.0f,0.0f,0.0f);
    light["bulb"] -> color = 1.5f * vec4(1.0f,0.2f,0.1f,1.0f);
    
    // Build the scene graph
    for (int i = 0; i < NUM_PLAYERS; i++) {
        node["player" + std::to_string(i)] = new Node("player" + std::to_string(i));

    }
    /*
    node["player0"] = new Node("player0");
    node["player1"] = new Node("player1");
    node["player2"] = new Node("player2");
    node["player3"] = new Node("player3");
    */

    node["pink_car"] = new Node("pink_car");
    node["pink_car"]->models.push_back(model["pink_car"]);
    node["pink_car"]->modeltransforms.push_back(mat4(1.0f));

    node["blue_car"] = new Node("pink_car");
    node["blue_car"]->models.push_back(model["blue_car"]);
    node["blue_car"]->modeltransforms.push_back(mat4(1.0f));

    node["yellow_car"] = new Node("yellow_car");
    node["yellow_car"]->models.push_back(model["yellow_car"]);
    node["yellow_car"]->modeltransforms.push_back(mat4(1.0f));

    node["green_car"] = new Node("green_car");
    node["green_car"]->models.push_back(model["green_car"]);
    node["green_car"]->modeltransforms.push_back(mat4(1.0f));

    // Tires
    node["p_tire_f"] = new Node("p_tire_f"); 
    node["p_tire_f"]->models.push_back(model["tire"]); 
    node["p_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    //node["p_tire_f"]->modeltransforms.push_back(rotate(135.0f * float(M_PI) / 180.0f, vec3(0.0f, 0.0f, 1.0f)));
    node["p_tire_b"] = new Node("p_tire_b"); 
    node["p_tire_b"]->models.push_back(model["tire"]); 
    node["p_tire_b"]->modeltransforms.push_back(mat4(1.0f));
    
    node["b_tire_f"] = new Node("b_tire_f"); 
    node["b_tire_f"]->models.push_back(model["tire"]); 
    node["b_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    node["b_tire_b"] = new Node("b_tire_b"); 
    node["b_tire_b"]->models.push_back(model["tire"]); 
    node["b_tire_b"]->modeltransforms.push_back(mat4(1.0f));

    node["y_tire_f"] = new Node("y_tire_f"); 
    node["y_tire_f"]->models.push_back(model["tire"]); 
    node["y_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    node["y_tire_b"] = new Node("y_tire_b"); 
    node["y_tire_b"]->models.push_back(model["tire"]); 
    node["y_tire_b"]->modeltransforms.push_back(mat4(1.0f));

    node["g_tire_f"] = new Node("g_tire_f"); 
    node["g_tire_f"]->models.push_back(model["tire"]); 
    node["g_tire_f"]->modeltransforms.push_back(mat4(1.0f));
    node["g_tire_b"] = new Node("g_tire_b"); 
    node["g_tire_b"]->models.push_back(model["tire"]); 
    node["g_tire_b"]->modeltransforms.push_back(mat4(1.0f));

    // Crowns 
    for (int i = 0; i < NUM_PLAYERS; i++) {
		node["crown" + std::to_string(i)] = new Node("crown" + std::to_string(i), false); 
		node["crown" + std::to_string(i)]->models.push_back(model["crown"]);
		node["crown" + std::to_string(i)]->modeltransforms.push_back(mat4(1.0f));
    }
    node["crown_world"] = new Node("crown_world", true); 
    node["crown_world"]->models.push_back(model["crown"]);
    node["crown_world"]->modeltransforms.push_back(mat4(1.0f));

    // Map
    node["map"] = new Node("map");
    node["map"]->models.push_back(model["map"]);
    node["map"]->modeltransforms.push_back(mat4(1.0f));

    //********************************************
    node["test_cube"] = new Node("test_cube");
    node["test_cube"]->models.push_back(model["cube"]); 
    node["test_cube"]->modeltransforms.push_back(mat4(1.0f));
    //********************************************

    node["particles0"] = new Node("test_cube", true, true);

    vec3 front_tire_translate = vec3(-1.25f, -0.65f, 0.0f);
    vec3 back_tire_translate = vec3(1.25f, -0.65f, 0.0f);
    mat4 front_tire_transform = translate(front_tire_translate); 
    mat4 back_tire_transform = translate(back_tire_translate);
    mat4 crown_transform = translate(vec3(0.0f, 0.8f, 0.0f)) * scale(vec3(1.2f, 1.2f, 1.2f));
    mat4 particle_transform = translate(vec3(1.25f, -0.4f, 0.0f));

    node["world"]->childnodes.push_back(node["player0"]);
    node["world"]->childtransforms.push_back(mat4(1.0f));
    node["player0"]->childnodes.push_back(node["pink_car"]);
    node["player0"]->childtransforms.push_back(translate(vec3(0.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate(-90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["pink_car"]->childnodes.push_back(node["p_tire_f"]);
    node["pink_car"] -> childtransforms.push_back( front_tire_transform);
    node["pink_car"]->childnodes.push_back(node["p_tire_b"]);
    node["pink_car"]->childtransforms.push_back(back_tire_transform);
    node["pink_car"]->childnodes.push_back(node["crown0"]);
    node["pink_car"]->childtransforms.push_back(crown_transform);
    node["pink_car"]->childnodes.push_back(node["particles0"]);
    node["pink_car"]->childtransforms.push_back(particle_transform);
    
    /*
    node["world"]->childnodes.push_back(node["player1"]);
    node["world"]->childtransforms.push_back(mat4(1.0f));
    node["player1"]->childnodes.push_back(node["blue_car"]);
    node["player1"]->childtransforms.push_back(translate(vec3(2.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate(-90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["blue_car"]->childnodes.push_back(node["b_tire_f"]);
    node["blue_car"] -> childtransforms.push_back(front_tire_transform );
    node["blue_car"]->childnodes.push_back(node["b_tire_b"]);
    node["blue_car"]->childtransforms.push_back(back_tire_transform);
    node["blue_car"]->childnodes.push_back(node["crown1"]);
    node["blue_car"]->childtransforms.push_back(crown_transform);

    node["world"]->childnodes.push_back(node["player2"]);
    node["world"]->childtransforms.push_back(mat4(1.0f));
    node["player2"]->childnodes.push_back(node["yellow_car"]);
    node["player2"]->childtransforms.push_back(translate(vec3(4.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate(-90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["yellow_car"]->childnodes.push_back(node["y_tire_f"]);
    node["yellow_car"] -> childtransforms.push_back(front_tire_transform );
    node["yellow_car"]->childnodes.push_back(node["y_tire_b"]);
    node["yellow_car"]->childtransforms.push_back(back_tire_transform);
    node["yellow_car"]->childnodes.push_back(node["crown2"]);
    node["yellow_car"]->childtransforms.push_back(crown_transform);

    node["world"]->childnodes.push_back(node["player3"]);
    node["world"]->childtransforms.push_back(mat4(1.0f));
    node["player3"]->childnodes.push_back(node["green_car"]);
    node["player3"]->childtransforms.push_back(translate(vec3(6.0f, 0.0f, 0.0f)) * scale(vec3(0.5f, 0.5f, 0.5)) * rotate(-90.0f*float(M_PI)/180.0f, vec3(0.0f, 1.0f, 0.0f)));
    node["green_car"]->childnodes.push_back(node["g_tire_f"]);
    node["green_car"] -> childtransforms.push_back(front_tire_transform);
    node["green_car"]->childnodes.push_back(node["g_tire_b"]);
    node["green_car"]->childtransforms.push_back(back_tire_transform);
    node["green_car"]->childnodes.push_back(node["crown3"]);
    node["green_car"]->childtransforms.push_back(crown_transform);

    node["world"]->childnodes.push_back(node["map"]); 
    node["world"]->childtransforms.push_back(translate(vec3(0.0f, -1.0f, 0.0f)));

    node["world"]->childnodes.push_back(node["crown_world"]); 
    node["world"]->childtransforms.push_back(translate(vec3(0.0f, 6.0f, 0.0f)));

    node["world"]->childnodes.push_back(node["test_cube"]); 
    node["world"]->childtransforms.push_back(translate(vec3(4.0f, 0.0f, 4.0f)) * scale(vec3(0.5f, 0.5f, 0.5f)));
    */
    
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
