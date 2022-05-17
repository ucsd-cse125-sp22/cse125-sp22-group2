#include "Game.h"

void Game::updateDrips(int time) {

	// translate the drips 
	float y_offset = sin(time * float(M_PI) / 5000.0f);

	glm::mat4 offset = translate(glm::vec3(0.0f, y_offset, 0.0f));

	drips->modeltransforms[0] = offset * initial_drip_transform;
}