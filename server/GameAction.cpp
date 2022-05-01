#include "GameAction.hpp"



cse125gameaction::GameActionTracker::GameActionTracker(unsigned int numPlayers)
{
	for (int i = 0; i < numPlayers; i++) {
		cse125gameaction::GameActionContainer* container = new cse125gameaction::GameActionContainer();
		this->tracker.insert({ i, container });
	}
}

cse125gameaction::GameActionTracker::~GameActionTracker()
{
	for (auto it = this->tracker.begin(); it != this->tracker.end(); it++) {
		// Delete dynamically allocated GameAction containers
		delete (it->second);
	}
}

void cse125gameaction::GameActionTracker::setAction(int playerId, MovementKey movementKey, vec3 cameraDirection)
{
	// Set the camera direction. This is the same regardless of the movement key
	cse125gameaction::GameActionContainer*& container = this->tracker.at(playerId);
	container->cameraDirection = cameraDirection;
	switch (movementKey) {
	case MovementKey::RIGHT:
		this->moveRight(playerId);
		break;
	case MovementKey::FORWARD:
		this->moveForward(playerId);
		break;
	case MovementKey::LEFT:
		this->moveLeft(playerId);
		break;
	case MovementKey::BACKWARD:
		this->moveBackward(playerId);
		break;
	default:
		break;		
	}
}

void cse125gameaction::GameActionTracker::moveRight(int playerId)
{
	GameActionContainer*& container = this->tracker.at(playerId);
	container->right = 1;
	container->left = 0;
}

void cse125gameaction::GameActionTracker::moveForward(int playerId)
{
	GameActionContainer*& container = this->tracker.at(playerId);
	container->forward = 1;
	container->backward = 0;
}

void cse125gameaction::GameActionTracker::moveLeft(int playerId)
{
	GameActionContainer*& container = this->tracker.at(playerId);
	container->right = 0;
	container->left = 1;
}

void cse125gameaction::GameActionTracker::moveBackward(int playerId)
{
	GameActionContainer*& container = this->tracker.at(playerId);
	container->forward = 0;
	container->backward = 1;
}

std::vector<cse125gameaction::GameAction> cse125gameaction::GameActionTracker::getActions(int playerId)
{
	std::vector<GameAction> actions;
	const GameActionContainer*& container = this->tracker.at(playerId);
	if (container->right) {
		actions.push_back(cse125gameaction::GameAction::MOVE_RIGHT);
	}
	if (container->forward) {
		actions.push_back(cse125gameaction::GameAction::MOVE_FORWARD);
	}
	if (container->left) {
		actions.push_back(cse125gameaction::GameAction::MOVE_LEFT);
	}
	if (container->backward) {
		actions.push_back(cse125gameaction::GameAction::MOVE_BACKWARD);
	}
	return actions;
}

vec3 cse125gameaction::GameActionTracker::getCameraDirection(int playerId)
{
	return this->tracker.at(playerId)->cameraDirection;
}
