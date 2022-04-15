#pragma once
// Represents a collection of data that is sent between the Client and Server as part
// of the core gameplay loop
#include <vector>
#include <boost/array.hpp>
#include "Definitions.hpp"
#include "Constants.hpp"

namespace cse125framing {

	enum class Action {
		NO_ACTION,
		MOVE,
		JUMP,
		LEAVE_TRAIL,
		REDO_MAKEUP,
		LOSE_CROWN,
		TAKE_CROWN
	};

	typedef struct Frame {
		vec4 playerPosition;
		vec3 movementDirection;
		RealNumber makeupLevel;
		RealNumber score;
		bool hasCrown;
		Action action;
	} Frame;

	/**
	 * @brief Serializes the Frame into the given buffer
	 * 
	 * @param frame   game state data
	 * @param buffer  game state data byte array output 
	 */
	void serialize(const Frame* frame, boost::array<char, cse125constants::FRAME_BUFFER_SIZE>& buffer);

	/**
	 * @brief Sets the fields of the Frame from the given buffer
	 * 
	 * @param frame   game state data
	 * @param buffer  game state data byte array input
	 */
	void deserialize(Frame* frame, const boost::array<char, cse125constants::FRAME_BUFFER_SIZE>& buffer);

	std::ostream& operator<<(std::ostream& os, const Action& action);
	std::ostream& operator<<(std::ostream& os, const Frame* frame);
	
}