/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "Scene.h"
#include "Cube.h"
#include "Obj.h"
#include "Debug.h"

// The scene init definition 
#include "Scene.inl"


using namespace glm;
void Scene::draw(void){
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    shader -> view = camera -> view;
    shader -> projection = camera -> proj;
    shader -> nlights = light.size();
    shader -> lightpositions.resize( shader -> nlights );
    shader -> lightcolors.resize( shader -> nlights );
    int count = 0;
    for ( std::pair<std::string,Light*> entry : light ){
        shader -> lightpositions[ count ] = (entry.second) -> position;
        shader -> lightcolors[ count ] = (entry.second) -> color;
        count++;
    }
    
    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    Node* cur = node["world"]; // root of the tree
    mat4 cur_VM = camera->view; // update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.

    // The following is the beginning of the depth-first search algorithm.
    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);
    while (!dfs_stack.empty()) {
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the size of the graph.
        // Note that, at any time, the stack does not contain repeated element.
        if (dfs_stack.size() > node.size()) {
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }

        // top-pop the stacks
        cur = dfs_stack.top();        dfs_stack.pop();
        cur_VM = matrix_stack.top(); matrix_stack.pop();
        // draw all the models at the current node
        for (unsigned int i = 0; i < cur->models.size(); i++) {
            // Prepare to draw the geometry. Assign the modelview and the material.

            shader->modelview = cur_VM * cur->modeltransforms[i]; // HW3: Without updating cur_VM, modelview would just be camera's view matrix.
            //shader->modelview = cur_VM * cur->modeltransforms[i] * translate(vec3(cur_VM[3][0], cur_VM[3][1], cur_VM[3][2])); // HW3: Without updating cur_VM, modelview would just be camera's view matrix.
            shader->material = (cur->models[i])->material;
            shader->texture_id = (cur->models[i])->geometry->object_number;
            shader->is_particle = 0;

            if (DEBUG_LEVEL >= LOG_LEVEL_FINER) {
                std::cout <<"Object number: " << (cur->models[i])->geometry->object_number << "\n";
            }

            // Check if the node is a particle source 
            if (cur->isParticleSource) {
                shader->is_particle = 1;
                // Update position of particle source and direction 
                cur->particles->Update(1, vec3(6.0f, 1.0f, 6.0f), vec3(0.0f, 0.0f, 1.0f), 0.1f, 5.0f, vec3(0.0f, 0.0f, 1.0f));

                // Draw particles
                cur->particles->Draw(mat4(1.0f), shader->program);
            }

            // The draw command
            shader->setUniforms();
            if (cur->visible) {
				(cur->models[i])->geometry->draw();
            }
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (unsigned int i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            matrix_stack.push(cur_VM * cur->childtransforms[i]);
        }

    } // End of DFS while loop.

}
