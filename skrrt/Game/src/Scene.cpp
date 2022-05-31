/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "Scene.h"
#include "Cube.h"
#include "Obj.h"
#include "Debug.h"
#include "TextureConstants.h"
#include "ShadowMapConstants.h"

// The scene init definition 
#include "Scene.inl"



using namespace glm;
void Scene::draw(Node* current_node){

    glUseProgram(shader->program);

    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    shader -> view = camera -> view;
    shader -> projection = camera -> proj;
    shader->sun = sun;

    shader -> numPointLights = pointLights.size();
    shader -> pointLights.resize( shader -> numPointLights );
    int count = 0;
    for ( std::pair<std::string,PointLight*> entry : pointLights ){
        shader->pointLights[count] = entry.second;
        count++;
    }
    
    shader -> numSpotLights = spotLights.size();
    shader -> spotLights.resize( shader -> numSpotLights );
    count = 0;
    for ( std::pair<std::string,SpotLight*> entry : spotLights ){
        shader->spotLights[count] = entry.second;
        count++;
    }

    // tell shader what the depth maps are
    if (ENABLE_SHADOW_MAP) {
        shader->directionalDepthMap = shadowMapOffset;
        for (int i = 0; i < spotDepthMaps.size(); i++) {
            shader->spotDepthMaps[i] = spotDepthMaps[i];
        }
        for (int i = 0; i < pointDepthMaps.size(); i++) {
            shader->pointDepthMaps[i] = pointDepthMaps[i];
        }
        shader->farPlane = CAMERA_FAR_PLANE;
        shader->viewPos = camera->eye;
    }

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    //Node* cur = node["world"]; // root of the tree
    Node* cur = current_node; // root of the tree
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

		// Check if the node is a particle source 
		if (cur->isParticleSource == 1) {

            //shader->modelview = cur_VM;
            shader->modelview = camera->view;
			shader->texture_id = 0;

			//shader->is_particle = 1;

			// The draw command
			shader->setUniforms();

			// Draw particles
			cur->particles->Draw(cur_VM, shader->program);
        }
        else {
			// draw all the models at the current node
			for (unsigned int i = 0; i < cur->models.size(); i++) {
				// Prepare to draw the geometry. Assign the modelview and the material.

				shader->modelview = cur_VM * cur->modeltransforms[i]; // HW3: Without updating cur_VM, modelview would just be camera's view matrix.
				shader->material = (cur->models[i])->material;
				shader->texture_id = (((cur->models[i])->geometry)->object_number) * NUM_TEXTURES + TEXTURE_OFFSET;
				shader->specular_id = (((cur->models[i])->geometry)->object_number) * NUM_TEXTURES + SPECULAR_OFFSET;
				shader->emission_id = (((cur->models[i])->geometry)->object_number) * NUM_TEXTURES + EMISSION_OFFSET;
				//shader->is_particle = 0;

				if (DEBUG_LEVEL >= LOG_LEVEL_FINER) {
					std::cout <<"Object number: " << (cur->models[i])->geometry->object_number << "\n";
				}

				// The draw command
				shader->setUniforms();

				if (cur->visible) {
					(cur->models[i])->geometry->draw(shader);
				}
			}
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (unsigned int i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            matrix_stack.push(cur_VM * cur->childtransforms[i]);
        }
    } // End of DFS while loop.
}

void Scene::drawDepthMap(Node* current_node){

    glUseProgram(depth_shader->program);

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    Node* cur = current_node; // root of the tree
    mat4 cur_VM = glm::mat4(1.0f); // update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.

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

		// Check if the node is a particle source 
		if (cur->isParticleSource == 1) {
            //do nothing
        }
        else {
			// draw all the models at the current node
			for (unsigned int i = 0; i < cur->models.size(); i++) {
				// Prepare to draw the geometry. Assign the modelview and the material.
                depth_shader->model = cur_VM * cur->modeltransforms[i];

				if (DEBUG_LEVEL >= LOG_LEVEL_FINER) {
					std::cout <<"Object number: " << (cur->models[i])->geometry->object_number << "\n";
				}

				// The draw command
				depth_shader->setUniforms();
				if (cur->visible) {
					(cur->models[i])->geometry->drawDepth(depth_shader);
				}
			}
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (unsigned int i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            matrix_stack.push(cur_VM * cur->childtransforms[i]);
        }
    } // End of DFS while loop.
}

void Scene::calculateShadowMaps() {

}

void Scene::updateScreen(void) {

    // Initial screen transformation 
    mat4 initial = translate(vec3(0.0f, 0.0f, camera->nearPlane - 0.1f)) * scale(vec3(0.001f, 0.001f, 0.001f));

    // Get camera transform 
    mat4 cur_VM = inverse(camera->view);

    // Update transform of UI root 
    node["UI_root"]->childtransforms[0] = cur_VM * initial;
}

void Scene::drawText(void) {
    glUseProgram(text_shader->program);

    text_shader->projection = glm::ortho(0.0f, cse125constants::WINDOW_WIDTH, 0.0f, cse125constants::WINDOW_HEIGHT);
    //text_shader->projection = glm::ortho(0.0f, cse125constants::WINDOW_HEIGHT, 0.0f, cse125constants::WINDOW_WIDTH);

    // Draw all scores 
    for (int i = 0; i < NUM_PLAYERS; i++) {
        scores[i]->setColor(text_colors[i]);
        text_shader->textColor = scores[i]->getColor(); 
		text_shader->setUniforms();
        scores[i]->RenderText();
        scores[i]->setPosition(cse125constants::WINDOW_WIDTH - 120.0f, cse125constants::WINDOW_HEIGHT - 50.0f * i - 75.0f);
    }

    // Draw game time
	text_shader->textColor = game_time->getColor(); 
    text_shader->setUniforms();
    game_time->RenderText();
	game_time->setPosition(cse125constants::WINDOW_WIDTH / 2.0f, cse125constants::WINDOW_HEIGHT - 75.0f);

}
