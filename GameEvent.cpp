#include "GameEvent.hpp"

gameevent::GameEvent::GameEvent()
{
	this->playerPosition = vec4(0);
	this->movementDirection = vec3(0);
	this->action = Action::NO_ACTION;
	this->hasCrown = false;
	this->makeupLevel = DEFAULT_SCORE;
	this->score = DEFAULT_MAKEUP_LEVEL;
}

gameevent::GameEvent::GameEvent(vec4 playerPosition, vec3 movementDirection, RealNumber makeupLevel, RealNumber score, bool hasCrown, Action action)
{
	this->playerPosition = playerPosition;
	this->movementDirection = movementDirection;
	this->makeupLevel = makeupLevel;
	this->score = score;
	this->hasCrown = hasCrown;
	this->action = action;
}

gameevent::GameEvent::~GameEvent()
{
}
