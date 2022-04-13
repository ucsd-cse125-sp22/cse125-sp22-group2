#pragma once
// Represents a collection of data that is sent between the Client and Server as part
// of the core gameplay loop
#include "Definitions.hpp"
#include "Constants.hpp"
using namespace constants;

namespace gameevent {

	enum class Action {
		NO_ACTION,
		MOVE,
		JUMP,
		LEAVE_TRAIL,
		REDO_MAKEUP,
		LOSE_CROWN,
		TAKE_CROWN
	};

	class GameEvent {
	public:
		vec4 playerPosition;
		vec3 movementDirection;
		RealNumber makeupLevel;
		RealNumber score;
		bool hasCrown;	
		Action action;
		GameEvent();
		GameEvent(vec4 playerPosition, vec3 movementDirection, RealNumber makeupLevel, RealNumber score, bool hasCrown, Action action);
		~GameEvent();
	};
}