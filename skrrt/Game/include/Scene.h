/**************************************************

*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>

#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <stack>

#include "Camera.h"
#include "SurfaceShader.h"
#include "Light.h"
#include "Geometry.h"
#include "Material.h"
#include "Model.h"
#include "ParticleSource.h"

#ifndef __SCENE_H__
#define __SCENE_H__

class Node {
public:
    Node(std::string n = "", bool v = true, bool p = false) { 
        name = n; 
        visible = v; 
        isParticleSource = p;
        if (p) {
			particles = &ParticleSource(); 
            std::cout << "Call Particle Source ctor" << std::endl;
        }
        else {
            particles = NULL;
        }
    };

    std::string name; 
    bool visible;
    bool isParticleSource; 

    ParticleSource* particles;

    std::vector< Node* > childnodes;
    std::vector< glm::mat4 > childtransforms;
    std::vector< Model* > models;
    std::vector< glm::mat4 > modeltransforms;

};

class Scene {
public:
    Camera* camera;
    SurfaceShader* shader;
    // The following are containers of objects serving as the object palettes.
    // The containers store pointers so that they can also store derived class objects.
    std::map< std::string, Geometry* > geometry;
    std::map< std::string, Material* > material;
    std::map< std::string, Model* > model;
    std::map< std::string, PointLight* > pointLights;
    std::map< std::string, SpotLight* > spotLights;
    DirectionalLight* sun;
    
    // The container of nodes will be the scene graph after we connect the nodes by setting the child_nodes.
    std::map< std::string, Node* > node;
    
    Scene(){
        // the default scene graph already has one node named "world."
        node["world"] = new Node("world");
        node["UI_root"] = new Node("UI_root");
    }
    
    void init( void );
    void draw(Node* current_node);

    void updateScreen(void);
    
    // destructor
    ~Scene(){
        // The containers of pointers own the object pointed to by the pointers.
        // All the objects should be deleted when the object palette is destructed.
        // light
        for(std::pair<std::string,PointLight*> entry : pointLights ){
            delete entry.second;
        }
        // geometry
        for(std::pair<std::string,Geometry*> entry : geometry ){
            delete entry.second;
        }
        // material
        for(std::pair<std::string,Material*> entry : material ){
            delete entry.second;
        }
        // model
        for(std::pair<std::string,Model*> entry : model ){
            delete entry.second;
        }
        // model
        for(std::pair<std::string,Node*> entry : node ){
            delete entry.second;
        }
        delete camera;
        delete shader;
        delete sun;
    }
};

#endif 
