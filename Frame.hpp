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

	std::ostream& operator<<(std::ostream& os, const Action& action);

	enum class MovementKey {
		FORWARD,
		BACKWARD,
		RIGHT,
		LEFT
	};

	std::ostream& operator<<(std::ostream& os, const MovementKey& movementKey);

	typedef struct ClientFrame {
		int id;
		int ctr;
		MovementKey movementKey;
		vec3 cameraDirection;
	} ClientFrame;

	const size_t CLIENT_FRAME_BUFFER_SIZE = sizeof(ClientFrame);

	std::ostream& operator<<(std::ostream& os, const ClientFrame* frame);

	/**
	 * @brief Serializes the Frame into the given buffer
	 *
	 * @param frame   game state data
	 * @param buffer  game state data byte array output
	 */
	void serialize(const ClientFrame* frame, boost::array<char, CLIENT_FRAME_BUFFER_SIZE>& buffer);

	/**
	 * @brief Sets the fields of the Frame from the given buffer
	 *
	 * @param frame   game state data
	 * @param buffer  game state data byte array input
	 */
	void deserialize(ClientFrame* frame, const boost::array<char, CLIENT_FRAME_BUFFER_SIZE>& buffer);

	// Data for individual player
	typedef struct PlayerData {
		vec4 playerPosition;
		vec3 playerDirection;
		RealNumber makeupLevel;
		RealNumber score;
		bool hasCrown;
	} PlayerData;

	// Only handles 1 client at a time currently
	// To extend, make each type a vector (e.g. of ids, player positions)
	typedef struct ServerFrame {
		int id; // index in players
		int ctr;
		PlayerData players[cse125constants::NUM_PLAYERS];
		RealNumber gameTime;
	} ServerFrame;

	const size_t SERVER_FRAME_BUFFER_SIZE = sizeof(ServerFrame);

	std::ostream& operator<<(std::ostream& os, const ServerFrame* frame);

	/**
	 * @brief Serializes the Frame into the given buffer
	 *
	 * @param frame   game state data
	 * @param buffer  game state data byte array output
	 */
	void serialize(const ServerFrame* frame, boost::array<char, SERVER_FRAME_BUFFER_SIZE>& buffer);

	/**
	 * @brief Sets the fields of the Frame from the given buffer
	 *
	 * @param frame   game state data
	 * @param buffer  game state data byte array input
	 */
	void deserialize(ServerFrame* frame, const boost::array<char, SERVER_FRAME_BUFFER_SIZE>& buffer);


}