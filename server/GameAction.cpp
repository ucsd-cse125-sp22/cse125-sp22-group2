#include "GameAction.hpp"



cse125gameaction::GameActionTracker::GameActionTracker(unsigned int numPlayers)
{
	for (unsigned int i = 0; i < numPlayers; i++) {
		cse125gameaction::GameActionContainer* container = new cse125gameaction::GameActionContainer{ 0, 0, 0, 0, 0 };
		this->tracker.push_back(container);
	}
}

cse125gameaction::GameActionTracker::~GameActionTracker()
{
	for (cse125gameaction::GameActionContainer* container : this->tracker)
	{
		delete container;
	}
}

void cse125gameaction::GameActionTracker::setAction(int playerId, MovementKey movementKey, vec3 cameraDirection)
{
	// Set the camera direction. This is the same regardless of the movement key
	cse125gameaction::GameActionContainer* container = this->tracker.at(playerId);
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
	case MovementKey::SPACE:
		this->moveAction(playerId);
		break;
	default:
		break;		
	}
}

void cse125gameaction::GameActionTracker::moveRight(int playerId)
{
	GameActionContainer* container = this->tracker.at(playerId);
	container->right = 1 ^ container->left;
	container->left = 0;
}

void cse125gameaction::GameActionTracker::moveForward(int playerId)
{
	GameActionContainer* container = this->tracker.at(playerId);
	container->forward = 1 ^ container->backward;
	container->backward = 0;
}

void cse125gameaction::GameActionTracker::moveLeft(int playerId)
{
	GameActionContainer* container = this->tracker.at(playerId);
	container->left = 1 ^ container->right;
	container->right = 0;
}

void cse125gameaction::GameActionTracker::moveBackward(int playerId)
{
	GameActionContainer* container = this->tracker.at(playerId);
	container->backward = 1 ^ container->forward;
	container->forward = 0;
}

void cse125gameaction::GameActionTracker::moveAction(int playerId)
{
	GameActionContainer* container = this->tracker.at(playerId);
	container->action = 1;
}

const cse125gameaction::GameActionContainer* cse125gameaction::GameActionTracker::getGameActionContainer(int playerId)
{
	return this->tracker.at(playerId);
}

GameAction cse125gameaction::gameActionFromContainer(const GameActionContainer*& gac)
{
	if (gac->action) {
		return GameAction::ACTION;
	}
	else if (gac->forward && gac->right) {
		return GameAction::MOVE_FORWARD_RIGHT;
	}
	else if (gac->forward && gac->left) {
		return GameAction::MOVE_FORWARD_LEFT;
	}
	else if (gac->backward && gac->left) {
		return GameAction::MOVE_BACKWARD_LEFT;
	}
	else if (gac->backward && gac->right) {
		return GameAction::MOVE_BACKWARD_RIGHT;
	}
	else if (gac->right) {
		return GameAction::MOVE_RIGHT;
	}
	else if (gac->forward) {
		return GameAction::MOVE_FORWARD;
	}
	else if (gac->left) {
		return GameAction::MOVE_LEFT;
	}
	else if (gac->backward) {
		return GameAction::MOVE_BACKWARD;
	}
	else {
		return GameAction::IDLE;
	}

}
