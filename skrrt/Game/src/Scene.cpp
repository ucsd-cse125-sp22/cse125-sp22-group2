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

        shader->particleColor = glm::vec3(0.0f, 0.0f, 0.0f);
		// Check if the node is a particle source 
		if (cur->isParticleSource == 1) {

            //shader->modelview = cur_VM;
            shader->modelview = camera->view;
			shader->texture_id = 0;

			//shader->is_particle = 1;

			// The draw command
			shader->setUniforms();

			// Draw particles
			cur->particles->Draw(cur_VM, shader, shader->program);
            cur->particlesPowerup->Draw(cur_VM, shader, shader->program);
        }
        else {
			// draw all the models at the current node
			for (unsigned int i = 0; i < cur->models.size(); i++) {
				// Prepare to draw the geometry. Assign the modelview and the material.
                shader->iFrames = cur->iframes;
                //shader->iFrames = cur->iframes;

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

void Scene::drawPart(Node* current_node){

    glUseProgram(part_shader->program);

    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    //camera -> computeMatrices();
    part_shader -> projection = camera -> proj;

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

        part_shader->particleColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		// Check if the node is a particle source 
		if (cur->isParticleSource == 1) {

            //part_shader->modelview = cur_VM;
            part_shader->modelview = camera->view;
			part_shader->setUniforms();

			// Draw particles
			cur->particles->Draw(cur_VM, part_shader, part_shader->program);
            cur->particlesPowerup->Draw(cur_VM, part_shader, part_shader->program);
        }
        else {
			// draw all the models at the current node
			for (unsigned int i = 0; i < cur->models.size(); i++) {
				// Prepare to draw the geometry. Assign the modelview and the material.
				part_shader->modelview = cur_VM * cur->modeltransforms[i]; // HW3: Without updating cur_VM, modelview would just be camera's view matrix.

				if (DEBUG_LEVEL >= LOG_LEVEL_FINER) {
					std::cout <<"Object number: " << (cur->models[i])->geometry->object_number << "\n";
				}

				// The draw command
				part_shader->setUniforms();

				if (cur->visible) {
					(cur->models[i])->geometry->draw(part_shader);
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

void Scene::drawNoPart(Node* current_node){

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

        shader->particleColor = glm::vec3(0.0f, 0.0f, 0.0f);
		// Check if the node is a particle source 
		if (cur->isParticleSource == 1) {
        }
        else {
			// draw all the models at the current node
			for (unsigned int i = 0; i < cur->models.size(); i++) {
				// Prepare to draw the geometry. Assign the modelview and the material.
                shader->iFrames = cur->iframes == 0.0f? 0.0f : 1.0 - (glm::mod(cur->iframes, 15.0f) / 15.0f);
                //shader->iFrames = cur->iframes;

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
    mat4 initial = translate(vec3(0.0f, 0.0f, camera->ui_near_default - 0.1f)) * scale(vec3(0.001f, 0.001f, 0.001f));

    // Get camera transform 
    mat4 cur_VM = inverse(camera->view);

    // Update transform of UI root 
    node["UI_root"]->childtransforms[0] = cur_VM * initial;

    // Update transform of UI root 
    // node["UI_logo"]->childtransforms[0] = cur_VM * initial;
}

void Scene::drawText(const bool& renderScores, const bool& renderTime, const bool& renderStartText, const bool& renderCountdownText, const bool& renderMatchEndText, 
                     const std::string& countdownText, const std::string& matchEndText) {
    glUseProgram(text_shader->program);

    int currentWidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentHeight = glutGet(GLUT_WINDOW_HEIGHT);

    text_shader->projection = glm::ortho(0.0f, (float)currentWidth, 0.0f, (float)currentHeight);
    //text_shader->projection = glm::ortho(0.0f, currentHeight, 0.0f, currentWidth);

    // Draw all scores 
    if (renderScores) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            scores[i]->setColor(text_colors[i]);
            text_shader->textColor = scores[i]->getColor();
            text_shader->setUniforms();
            //scores[i]->setPosition(currentWidth - 120.0f, currentHeight - 50.0f * i - 75.0f);
            scores[i]->setPosition(124.0f, currentHeight - 75.0f * i - 100.0f);
            scores[i]->RenderText();
        }
    }

    if (renderTime) {
        // Draw game time
        text_shader->textColor = game_time->getColor();
        text_shader->setUniforms();
        game_time->setPosition(currentWidth / 2.0f + 2.0f, currentHeight - 75.0f);
        game_time->RenderTextCenter();
    }

    // Draw start text
    if (renderStartText) {
        if (flashStartTextCount < flashStartTextFrequency) {
            if (showStartText) {
                text_shader->textColor = start_text->getColor();
                text_shader->setUniforms();
                start_text->setPosition(currentWidth / 2.0f, currentHeight - 1000.0f);
                start_text->RenderTextCenter();
            }         
            flashStartTextCount += 1;
        }
        else {
            flashStartTextCount = 0;
            showStartText = !showStartText;
        }
    }

    // Draw countdown text
    if (renderCountdownText) {
        text_shader->textColor = countdown_instructions_text->getColor();
        text_shader->setUniforms();
        countdown_instructions_text->setPosition(currentWidth / 2.0f, currentHeight - 200.0f);
        countdown_instructions_text->RenderTextCenter();

        text_shader->textColor = countdown_go_text->getColor();
        text_shader->setUniforms();
        countdown_go_text->updateText(countdownText);
        countdown_go_text->setPosition(currentWidth / 2.0f, currentHeight - 400.0f);
        countdown_go_text->RenderTextCenter();
    }            

    // Draw end-of-match text
    if (renderMatchEndText) {
        text_shader->textColor = match_end_text->getColor();
        text_shader->setUniforms();
        match_end_text->updateText(matchEndText);
        match_end_text->setPosition(currentWidth / 2.0f, currentHeight - 200.0f);
        match_end_text->RenderTextCenter();
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

void Scene::drawUI(const bool& showStartLogo, const bool& showTimer, const bool& showMascara, const bool& showWheels, const bool& showCrownIcons) {
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

            // Hack to not draw certain UI elements
            if (!showStartLogo && cur->name == "logo") {
                continue;
            }

            if (!showMascara && (cur->name == "mascara_bar" || cur->name == "mascara_icon" || cur->name == "white_bar")) {
                continue;
            }

            if (!showWheels && (cur->name == "pink_tire" || cur->name == "blue_tire" || cur->name == "green_tire" || cur->name == "yellow_tire")) {
                continue;
            }

            if (!showTimer && (cur->name == "clock")) {
                continue;
            }

            // Check if string starts with 'crown_icon'
            if (!showCrownIcons && (cur->name.find("crown_icon") == 0)) {
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

// clamps time to [0.0f, 1.0f]. 0.0f is day, 1.0f is night
void Scene::setDayNight(float timeDayNight) {
    float time_c = glm::clamp(timeDayNight, 0.0f, 1.0f);
    setPointLights(glm::mix(nightPointBrightness, dayPointBrightness, 1.0f - time_c));
    setSpotLights(glm::mix(nightSpotBrightness, daySpotBrightness, 1.0f - time_c));
    setSun(glm::mix(nightSunBrightness, daySunBrightness, 1.0f - time_c));

    sun->ambient = glm::mix(sun_night->ambient, sun_day->ambient, time_c);
    sun->diffuse = glm::mix(sun_night->diffuse, sun_day->diffuse, time_c);
    sun->specular = glm::mix(sun_night->specular, sun_day->specular, time_c);
    sun->direction = glm::mix(sun_night->direction, sun_day->direction, time_c);
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

void Scene::setSun(float brightness) {
	sun->ambient = brightness*sun->ambient;
	sun->diffuse = brightness*sun->diffuse;
	sun->specular = brightness*sun->specular;
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
	const float logo_width_to_height_ratio = 9119.0f / 4988.0f; // determined from the image dimensions
	const float logo_scale = 10.0f; // tune according to window dimensions
	//start menu
	node["screen"]->childtransforms[10] = (translate(vec3(0.0f, 0.0f, 1.0f))* scale(vec3(logo_scale* logo_width_to_height_ratio, logo_scale, 1.0f)));
}
