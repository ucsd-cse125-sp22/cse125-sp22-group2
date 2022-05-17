#include "Game.h"

void Game::updateDrips(int time, RealNumber makeupLevel) {

	// translate the drips linearly based on makeupLevel
	//float y_offset = sin(time * float(M_PI) / 5000.0f);
	float maximumOffset = 20.0f;
	//float maximumOffset = 0.0f;
	float y_offset = (100.0f - makeupLevel) * maximumOffset / 1000.0f - 1.2; 

	glm::mat4 offset = translate(glm::vec3(0.0f, -y_offset, 0.0f));

	std::cout << offset[3][1] << std::endl;

	drips->modeltransforms[0] = offset * initial_drip_transform;
}