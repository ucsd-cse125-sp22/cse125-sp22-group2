#define GLM_FORCE_RADIANS
#define _USE_MATH_DEFINES
#ifndef __GAME_H__
#define __GAME_H__

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>

#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <stack>

#include "Player.h"
#include "Geometry.h"
#include "Material.h"
#include "Model.h"
#include "Animation.h"

#include "AudioEngine.h"

#include "../../../Constants.hpp"
#include "../../../Definitions.hpp"

class Game {
private:
    float game_time = 0;

    // Add other parameters as needed
    Node* UI_root = nullptr;
    Node* drips = nullptr;
    glm::mat4 initial_drip_transform;

    Node* makeup_status_bar = nullptr; 
    glm::mat4 initial_makeup_transform;

    Node* blowdryer_status_icon = nullptr;
    Node* blowdryer_pickup_node = nullptr;
    glm::mat4 blowdryer_translation = glm::mat4(1.0f);
    glm::mat4 initial_blowdryer_transform = glm::mat4(1.0f);

    std::vector<Node*> makeup_gate_arms;
    std::vector<glm::mat4> initial_arm_transforms;

    //Animation gateAnimation;
    std::map <std::string, Animation*> animations; 
    AudioEngine audioEngine;
    std::map<int, int> carEngineChannels;
    void applyAnimations(); 

public: 
    std::vector<int> scores; 
    std::vector<Player*> players; 

    Game(int numPlayers) {
        for (int i = 0; i < numPlayers; i++) {
            players.push_back(new Player());
            scores.push_back(0);
        }
    }

    ~Game() {
        for (Player* p : players) {
            delete(p); 
        }
    }

    void init(Node* world, Node* ui_root) {
        UI_root = ui_root; 


        // Find drips
        for (Node* child : ui_root->childnodes[0]->childnodes) {
            if (child->name == "drips") {
                drips = child;
            }

            if (child->name == "white_bar") {
                makeup_status_bar = child;
            }

            if (child->name == "blowdryer_icon") {
                blowdryer_status_icon = child;
            }

        }

        initial_drip_transform = drips->modeltransforms[0];
        initial_makeup_transform = makeup_status_bar->modeltransforms[0]; 

        // Find makeup gate arms
        for (Node* child : world->childnodes) {
            // Find blowdryer pick up
            if (child->name == "blowdryer_world") {
                blowdryer_pickup_node = child;
                initial_blowdryer_transform = child->modeltransforms[0];
            }

            // Save makeup gate arms
            if (child->name.find("makeup_station") != std::string::npos) {
                makeup_gate_arms.push_back(child->childnodes[0]); 
                initial_arm_transforms.push_back(child->childnodes[0]->modeltransforms[0]);
            }
        }

        // Read in animations 
        parseGateAnimation(); 
        parseCarCollisionAnimation();
        
    }

    void updateTime(RealNumber t) { game_time = t; }
    float getTime() { return game_time; };

    void updateDrips(int time, RealNumber makeupLevel); 
    void updateMakeupStatusBar(int time, RealNumber makeupLevel);
    void updateBlowdryerIcon(bool visible);
    void setBlowdryerTransform(glm::mat4 t) { blowdryer_translation = t; }
    void bobPowerup(int time);
    void updateAnimations(); 

    void parseGateAnimation();
    void triggerGateAnimation(int gateNum);

    void parseCarCollisionAnimation();
    void triggerCarCollisionAnimation(int playerNum);

    /* AUDIO TRIGGERS */
    void stopAllSounds();
    void playMusic(const char* soundName, float db = 0.0);
    //void stopMusic(const char* soundName);
    void triggerFx(const char* fxName, const vec3& position = { 0,0,0 }, float dB = 0.0);

    vec3 computeCamRelative3dPosition(const vec3& cameraPos, const vec3& playerPos, const vec3& eventPos);

};


#endif
