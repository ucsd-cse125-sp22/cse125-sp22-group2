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
#include "TextShader.h"
#include "Text.h"
#include "UIShader.h"

#include "../../../Constants.hpp"

//#define NUM_PLAYERS 4

#ifndef __SCENE_H__
#define __SCENE_H__

class Node {
public:
    Node(std::string n = "", bool v = true, bool p = false) { 
        name = n; 
        visible = v; 
        isParticleSource = p;
        if (p) {
			particles = new ParticleSource(); 
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
    TextShader* text_shader;
    UIShader* ui_shader;

    Text* scores[4];
    Text* game_time;
    Text* countdown_instructions_text;
    Text* countdown_go_text;
    Text* match_end_text;

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

    glm::vec3 text_colors[4] = {glm::vec3(0.84f, 0.24f, 0.74f),  // pink
                                glm::vec3(0.31f, 0.68f, 0.89f),  // blue
                                glm::vec3(0.91f, 0.90f, 0.32f),  // yellow
                                glm::vec3(0.41f, 0.76f, 0.24f)}; // green
    
    /**
     * @brief Draws the text-based UI elements on the screen
     *
     * @param countdownTimeRemaining the amount of time left in the countdown
     * @param renderMatchEndText whether to render the end of match text
     * @param matchEndText       the end of match text to render
     */
    void drawText(const float& countdownTimeRemaining, const bool& renderMatchEndText, const std::string& matchEndText = "");
    void drawUI(void); 

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

        for (Text* t : scores) {
            delete t;
        }

        delete game_time;
        delete camera;
        delete shader;
        delete text_shader;
        delete ui_shader;
        delete sun;
    }
};

#endif 
