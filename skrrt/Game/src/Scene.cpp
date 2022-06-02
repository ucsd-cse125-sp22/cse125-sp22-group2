/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "Scene.h"
#include "Cube.h"
#include "Obj.h"
#include "Debug.h"
#include "TextureConstants.h"
#include "ShadowMapConstants.h"
#include "../../../Config.hpp"

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
    }
    shader->iFrames = 0.0f;

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
            cur->particlesPowerup->Draw(cur_VM, shader->program);
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

void Scene::drawDepthMap(Node* current_node, glm::mat4 lightSpace){

    glUseProgram(depth_shader->program);

    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    depth_shader->lightSpace = lightSpace;

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

void Scene::drawText(const float& countdownTimeRemaining, const bool& renderMatchEndText, const std::string& matchEndText) {
    glUseProgram(text_shader->program);

    int currentWidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentHeight = glutGet(GLUT_WINDOW_HEIGHT);

    text_shader->projection = glm::ortho(0.0f, (float)currentWidth, 0.0f, (float)currentHeight);
    //text_shader->projection = glm::ortho(0.0f, currentHeight, 0.0f, currentWidth);

    // Draw all scores 
    for (int i = 0; i < NUM_PLAYERS; i++) {
        scores[i]->setColor(text_colors[i]);
        text_shader->textColor = scores[i]->getColor(); 
		text_shader->setUniforms();
        //scores[i]->setPosition(currentWidth - 120.0f, currentHeight - 50.0f * i - 75.0f);
        scores[i]->setPosition(120.0f, currentHeight - 75.0f * i - 100.0f);
        scores[i]->RenderText();
    }

    // Draw game time
	text_shader->textColor = game_time->getColor(); 
    text_shader->setUniforms();
	game_time->setPosition(currentWidth / 2.0f, currentHeight - 75.0f);
    game_time->RenderText();

    // Draw countdown timer text
    if (countdownTimeRemaining > 0) {
        text_shader->textColor = countdown_instructions_text->getColor();
        text_shader->setUniforms();
        countdown_instructions_text->setPosition(currentWidth / 4.0f, currentHeight - 200.0f);
        countdown_instructions_text->RenderText();

        const int secondsLeft = (int)(countdownTimeRemaining / cse125config::TICK_RATE);

        // Determine the number of ellipses to display to simulate animation
        // Each 1/3 of a whole number corresponds to one ellipsis
        float secondsInteger = 0.0f;
        float secondsFraction = 1.0f - modf(countdownTimeRemaining / cse125config::TICK_RATE, &secondsInteger);
        std::string ellipses = "";
        if (secondsFraction >= 0.0f) {
            ellipses += ".";
        }
        if (secondsFraction >= 0.33f) {
            ellipses += ".";
        }
        if (secondsFraction >= 0.67f) {
            ellipses += ".";
        }

        switch (secondsLeft) {
        case 2:
            text_shader->textColor = countdown_go_text->getColor();
            text_shader->setUniforms();
            countdown_go_text->updateText("READY " + ellipses);
            countdown_go_text->setPosition(currentWidth / 2.0f, currentHeight - 400.0f);
            countdown_go_text->RenderText();
        case 1:
            text_shader->textColor = countdown_go_text->getColor();
            text_shader->setUniforms();
            countdown_go_text->updateText("SET " + ellipses);
            countdown_go_text->setPosition(currentWidth / 2.0f, currentHeight - 400.0f);
            countdown_go_text->RenderText();
            break;
        case 0:
            text_shader->textColor = countdown_go_text->getColor();
            text_shader->setUniforms();
            countdown_go_text->updateText("GO!");
            countdown_go_text->setPosition(currentWidth / 2.0f, currentHeight - 400.0f);
            countdown_go_text->RenderText();
            break;
        default:
            break;
        }
    }
    

    // Draw end-of-match text
    if (renderMatchEndText) {
        text_shader->textColor = match_end_text->getColor();
        text_shader->setUniforms();
        match_end_text->updateText(matchEndText);
        match_end_text->setPosition(currentWidth / 4.0f, currentHeight - 200.0f);
        match_end_text->RenderText();
    }
}

void Scene::drawDrips(void) {
    glUseProgram(ui_shader->program);

    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    ui_shader -> projection = camera -> proj;

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    Node* cur = node["UI_root"]; // root of the UI tree
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
            if (cur->name != "drips") {
                continue;
            }
			// Prepare to draw the geometry. Assign the modelview and the material.

			ui_shader->modelview = cur_VM * cur->modeltransforms[i]; // HW3: Without updating cur_VM, modelview would just be camera's view matrix.
			ui_shader->texture_id = (((cur->models[i])->geometry)->object_number) * NUM_TEXTURES + TEXTURE_OFFSET;

			if (DEBUG_LEVEL >= LOG_LEVEL_FINER) {
				std::cout <<"Object number: " << (cur->models[i])->geometry->object_number << "\n";
			}

			// The draw command
			ui_shader->setUniforms();

			if (cur->visible) {
				(cur->models[i])->geometry->draw(ui_shader);
			}
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (unsigned int i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            matrix_stack.push(cur_VM * cur->childtransforms[i]);
        }

    } // End of DFS while loop.
}

void Scene::drawUI(void) {
    glUseProgram(ui_shader->program);

    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    ui_shader -> projection = camera -> proj;

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    Node* cur = node["UI_root"]; // root of the UI tree
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
            if (cur->name == "drips") {
                continue;
            }
			// Prepare to draw the geometry. Assign the modelview and the material.

			ui_shader->modelview = cur_VM * cur->modeltransforms[i]; // HW3: Without updating cur_VM, modelview would just be camera's view matrix.
			ui_shader->texture_id = (((cur->models[i])->geometry)->object_number) * NUM_TEXTURES + TEXTURE_OFFSET;

			if (DEBUG_LEVEL >= LOG_LEVEL_FINER) {
				std::cout <<"Object number: " << (cur->models[i])->geometry->object_number << "\n";
			}

			// The draw command
			ui_shader->setUniforms();

			if (cur->visible) {
				(cur->models[i])->geometry->draw(ui_shader);
			}
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (unsigned int i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            matrix_stack.push(cur_VM * cur->childtransforms[i]);
        }

    } // End of DFS while loop.
}

void Scene::setSpotLights(float brightness) {
	for (std::pair<std::string, SpotLight*> entry : spotLights_init) {
		spotLights[entry.first]->ambient = brightness * entry.second->ambient;
		spotLights[entry.first]->diffuse = brightness * entry.second->diffuse;
		spotLights[entry.first]->specular = brightness * entry.second->specular;
	}
}

void Scene::setPointLights(float brightness) {
	for (std::pair<std::string, PointLight*> entry : pointLights_init) {
		pointLights[entry.first]->ambient = brightness * entry.second->ambient;
		pointLights[entry.first]->diffuse = brightness * entry.second->diffuse;
		pointLights[entry.first]->specular = brightness * entry.second->specular;
	}
}

void Scene::setSun(float brightness, bool sunOn) {
	if (sunOn) { 
		sun->direction = sun_day->direction;
		sun->ambient = brightness*sun_day->ambient;
		sun->diffuse = brightness*sun_day->diffuse;
		sun->specular = brightness*sun_day->specular;
	} else {
		sun->direction = sun_night->direction;
		sun->ambient = brightness*sun_night->ambient;
		sun->diffuse = brightness*sun_night->diffuse;
		sun->specular = brightness*sun_night->specular;
	}
}

void Scene::scaleUi(int width, int height) {
	const float inital_width = 1920.0f;
	const float inital_height = 1080.0f;

	const float heightRatio = (float) width/ inital_width;
	const float widthRatio = (float) height/inital_height;

	//screen
	node["UI_root"]->childtransforms[0] = (glm::mat4(1.0f));

	const float clock_ratio = 85.0f / 101.0f; 
	//clock
	node["screen"]->childtransforms[0]=(translate(vec3(-3.0f * widthRatio, 21.5f* heightRatio, 0.0f))* scale(1.4f * vec3(clock_ratio, 1.0f, 0.0f)));

	const float tire_icon_ratio = 65.0f / 64.0f;
	//pink tire
	node["screen"]->childtransforms[1] = (translate(vec3(-39.0f * widthRatio, 20.4f * heightRatio, 0.0f))* scale(1.0f * vec3(tire_icon_ratio, 1.0f, 0.0f)));
	//blue tire
	node["screen"]->childtransforms[2] = (translate(vec3(-39.0f * widthRatio, 17.1f * heightRatio, 0.0f))* scale(1.0f * vec3(tire_icon_ratio, 1.0f, 0.0f)));
	//yellow tire
	node["screen"]->childtransforms[3] = (translate(vec3(-39.0f * widthRatio, 13.8f * heightRatio, 0.0f))* scale(1.0f * vec3(tire_icon_ratio, 1.0f, 0.0f)));
	//green tire
	node["screen"]->childtransforms[4] = (translate(vec3(-39.0f * widthRatio, 10.5f * heightRatio, 0.0f))* scale(1.0f * vec3(tire_icon_ratio, 1.0f, 0.0f)));

	const float blowdryer_icon_ratio = 3227.0f / 3076.0f;
	//blowdrier icon
	node["screen"]->childtransforms[5] = (translate(vec3(36.0f * widthRatio, -16.0f * heightRatio, 0.0f))* scale(2.5f * vec3(blowdryer_icon_ratio, 1.0f, 0.0f)));

	const float mascara_icon_ratio = 179.0f / 177.0f; 
	const float mascara_bar_ratio = 557.0f / 70.0f;
	//mascera icon
	node["screen"]->childtransforms[6] = (translate(vec3(-10.0f * widthRatio, -20.0f * heightRatio, 0.0f))* scale(1.0f * vec3(mascara_icon_ratio, 1.0f, 0.0f)));
	//mascera bar
	node["screen"]->childtransforms[7] = (translate(vec3(1.0f * widthRatio, -20.0f * heightRatio, 0.0f))* scale(1.0f * vec3(mascara_bar_ratio, 1.0f, 0.0f)));
	//white bar
	node["screen"]->childtransforms[8] = (translate(vec3(-6.8f * widthRatio, -20.0f * heightRatio, -0.1f))* scale(0.98f * vec3(mascara_bar_ratio, 0.82f, 0.0f)));

	//drips
	node["screen"]->childtransforms[9] = (translate(vec3(0.0f * widthRatio, 0.0f * heightRatio, -1.0f))* scale(vec3(70.0f, 600.0f, 1.0f)));

	// TODO: Dynamic scaling? based on window size
	const float START_MENU_WIDTH_TO_HEIGHT_RATIO = 1780.0f / 1003.0f; // determined from the image dimensions
	const float START_MENU_SCALE = 15.0f; // tune according to window dimensions
	//start menu
	node["screen"]->childtransforms[10] = (translate(vec3(0.0f, 0.0f, 1.0f))* scale(vec3(START_MENU_SCALE* START_MENU_WIDTH_TO_HEIGHT_RATIO, START_MENU_SCALE, 1.0f)));
}
