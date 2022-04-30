#include "GameAction.hpp"

cse125gameaction::GameAction::GameAction()
{
	this->flags = 0;
}

cse125gameaction::GameAction::~GameAction()
{
}

void cse125gameaction::GameAction::setAction(const unsigned int& action)
{
	this->flags |= action;
}

void cse125gameaction::GameAction::clearAction(const unsigned int& action)
{
	this->flags &= (~action);
}

void cse125gameaction::GameAction::moveRight()
{
	this->setAction(cse125gameaction::MOVE_RIGHT);
	this->clearAction(cse125gameaction::MOVE_LEFT);
}

void cse125gameaction::GameAction::moveFoward()
{
	this->setAction(cse125gameaction::MOVE_FORWARD);
	this->clearAction(cse125gameaction::MOVE_BACKWARD);
}

void cse125gameaction::GameAction::moveLeft()
{
	this->setAction(cse125gameaction::MOVE_LEFT);
	this->clearAction(cse125gameaction::MOVE_RIGHT);
}

void cse125gameaction::GameAction::moveBackward()
{
	this->setAction(cse125gameaction::MOVE_BACKWARD);
	this->clearAction(cse125gameaction::MOVE_FORWARD);
}

std::vector<unsigned int> cse125gameaction::GameAction::getActions()
{
	// For each game action, check if it is set. If so, add it to the vector of actions that are active
	std::vector<unsigned int> activeActions;
	for (auto it = cse125gameaction::allActions.begin(); it != cse125gameaction::allActions.end(); it++) {
		const unsigned int& action = *it;
		if ((this->flags & action) == action) {
			activeActions.push_back(action);
		}
	}
	return activeActions;
}
