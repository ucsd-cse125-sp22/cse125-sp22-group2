//#include "../Definitions.hpp"
//#include "../Frame.hpp"
//#include "../GameLogic/PhysicalObjectManager.hpp"
//
//void gameLoop(PhysicalObjectManager& manager, int clientID, cse125framing::MovementKey movementKey, vec3 cameraDirection) {
//	PhysicalObject* player = manager.objects->at(clientID);
//	switch (movementKey) {
//	case cse125framing::MovementKey::RIGHT:
//		player->moveDirection(glm::normalize(vec3(-cameraDirection.x, cameraDirection.y, cameraDirection.z)));
//		break;
//	case cse125framing::MovementKey::FORWARD:
//		player->moveDirection(glm::normalize(cameraDirection));
//		break;
//	case cse125framing::MovementKey::LEFT:
//		player->moveDirection(glm::normalize(vec3(cameraDirection.x, cameraDirection.y, -cameraDirection.z)));
//		break;
//	case cse125framing::MovementKey::BACKWARD:
//		player->moveDirection(glm::normalize(-cameraDirection));
//		break;
//	}
//}