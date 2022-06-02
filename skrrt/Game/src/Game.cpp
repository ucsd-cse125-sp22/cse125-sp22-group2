#include "Game.h"

void Game::updateDrips(int time, RealNumber makeupLevel) {

	// translate the drips linearly based on makeupLevel
	//float y_offset = sin(time * float(M_PI) / 5000.0f);
	float maximumOffset = 20.0f;
	//float maximumOffset = 0.0f;
	float y_offset = (100.0f - makeupLevel) * maximumOffset / 1000.0f - 1.2; 

	glm::mat4 offset = translate(glm::vec3(0.0f, -y_offset, 0.0f));

	//std::cout << offset[3][1] << std::endl;

	drips->modeltransforms[0] = offset * initial_drip_transform;
}

void Game::updateMakeupStatusBar(int time, RealNumber makeupLevel) {

	// Scale makeup status bar based on the makeup level
	//glm::mat4 scale = glm::scale(glm::vec3(((float)makeupLevel / 100.0f), 1.0f, 1.0f));

	//makeup_status_bar->modeltransforms[0] = scale * initial_drip_transform;

	float max_offset = -1.66f; 
	glm::mat4 translate = glm::translate(glm::vec3(max_offset * (1 - (float)makeupLevel / 100.0f), 0.0f, 0.0f));
	makeup_status_bar->modeltransforms[0] = translate * initial_drip_transform;

}

// *******************************************
// ************* Gate Animation **************
// *******************************************

void Game::parseGateAnimation() {

	const char* path = "animations/makeup_gate.txt";
	const char* lipstick_path = "animations/lipstick.txt";
	const char* mascara_path = "animations/mascara.txt";
	const char* powder_path = "animations/powder_brush.txt";

	for (int i = 0; i < cse125constants::NUM_MAKEUP_STATIONS; i++) {
		animations["gate_anim" + std::to_string(i)] = new Animation();
		animations["gate_anim" + std::to_string(i)]->readAnimation(path);

		animations["lipstick_anim" + std::to_string(i)] = new Animation(); 
		animations["lipstick_anim" + std::to_string(i)]->readAnimation(lipstick_path);

		animations["mascara_anim" + std::to_string(i)] = new Animation(); 
		animations["mascara_anim" + std::to_string(i)]->readAnimation(mascara_path);

		animations["powder_anim" + std::to_string(i)] = new Animation(); 
		animations["powder_anim" + std::to_string(i)]->readAnimation(powder_path);
	}
	
	std::cout << "Successfully read in gate animation" << std::endl;
}

/* 
 * Function to trigger gate animation. 
 */
void Game::triggerGateAnimation(int gateNum) {
	animations["gate_anim" + std::to_string(gateNum)]->triggerAnimation(true);
	animations["lipstick_anim" + std::to_string(gateNum)]->triggerAnimation(true);
	animations["mascara_anim" + std::to_string(gateNum)]->triggerAnimation(true);
	animations["powder_anim" + std::to_string(gateNum)]->triggerAnimation(true);
}

// *******************************************
// ******** Car Collision Animation **********
// *******************************************

void Game::parseCarCollisionAnimation() {

	//const char* path = "animations/makeup_gate.txt";
	const char* path = "animations/car_collision.txt"; 

	//for (int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
	for (int i = 0; i < 4; i++) {
		animations["collision_anim" + std::to_string(i)] = new Animation();
		animations["collision_anim" + std::to_string(i)]->readAnimation(path); 
	}
	std::cout << "Successfully read in car collision animation" << std::endl;
}

void Game::triggerCarCollisionAnimation(int playerNum) {

	// Select which car to apply the animation on 
	animations["collision_anim" + std::to_string(playerNum)]->triggerAnimation(true);
}

void Game::updateAnimations() {

	std::map<std::string, Animation*>::iterator it;

	// Call update on all animations in map
	for (it = animations.begin(); it != animations.end(); it++) {
		it->second->updateAnimation();
	}

	// apply all animations to respective objects 
	applyAnimations(); 
}

void Game::applyAnimations() {

	// Gate's arm animation 
	// Obtain the frame's rotation, translation, and scale matrices 
	for (int i = 0; i < cse125constants::NUM_MAKEUP_STATIONS; i++) {
		glm::mat4 new_transformation = animations["gate_anim" + std::to_string(i)]->getCurrentTransform();

		// Apply the transformations to the gate's arm
		makeup_gate_arms[i]->modeltransforms[0] = initial_arm_transforms[i] * new_transformation;

		new_transformation = animations["lipstick_anim" + std::to_string(i)]->getCurrentTransform(); 
		lipsticks[i]->modeltransforms[0] = initial_lipstick_transforms[i] * new_transformation;

		new_transformation = animations["mascara_anim" + std::to_string(i)]->getCurrentTransform(); 
		mascaras[i]->modeltransforms[0] = initial_mascaras_transforms[i] * new_transformation;

		new_transformation = animations["powder_anim" + std::to_string(i)]->getCurrentTransform(); 
		powder_brushes[i]->modeltransforms[0] = initial_powder_transforms[i] * new_transformation;
	}

	// Car collision animations 
	for (int i = 0; i < 4; i++) {
		glm::mat4 new_transformation = animations["collision_anim" + std::to_string(i)]->getCurrentTransform(); 
		players[i]->setPlayerTransform(new_transformation); 
	}

	// *********************************
	// Add future animation applications here 
	// *********************************
}
