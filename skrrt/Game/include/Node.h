#pragma once

/*
class Node {
public:
    Node(std::string n = "", bool v = true, bool p = false) { 
        name = n; 
        visible = v; 
        isParticleSource = p;
        if (p) {
			particles = new ParticleSource();
            //particlesPowerup = new ParticleSource(8, 20 * 4 + 6, 2, 0, 6);
            particlesPowerup = new ParticleSource();
            std::cout << "Call Particle Source ctor" << std::endl;
        }
        else {
            particles = NULL;
            particlesPowerup = NULL;
        }
    };

    std::string name; 
    bool visible;
    bool isParticleSource; 

    ParticleSource* particles;
    ParticleSource* particlesPowerup;

    std::vector< Node* > childnodes;
    std::vector< glm::mat4 > childtransforms;
    std::vector< Model* > models;
    std::vector< glm::mat4 > modeltransforms;

};
*/
