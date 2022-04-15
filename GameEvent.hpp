#pragma once
// Represents a collection of data that is sent between the Client and Server as part
// of the core gameplay loop
#include "Definitions.hpp"
#include "Constants.hpp"
using namespace cse125constants;

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

		/**
		 * @brief Construct the default Game Event object
		 * 
		 */
		GameEvent();

		/**
		 * @brief Construct a new Game Event object
		 * 
		 * @param playerPosition     (x,y,z) coordinates, w translation 
		 * @param movementDirection  delta x, y, z
		 * @param makeupLevel        player makeup level
		 * @param score 	     player score
		 * @param hasCrown           if player has crown
		 * @param action             player action
		 */
		GameEvent(vec4 playerPosition, vec3 movementDirection, RealNumber makeupLevel, RealNumber score, bool hasCrown, Action action);

		/**
		 * @brief Destroy the Game Event object
		 * 
		 */
		~GameEvent();
	};
}