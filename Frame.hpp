#pragma once
// Represents a collection of data that is sent between the Client and Server as part
// of the core gameplay loop
#include <vector>
#include <boost/array.hpp>
#include "Definitions.hpp"
#include "Constants.hpp"

namespace cse125framing {

	enum class Action {
		NO_ACTION = -1,
		MOVE,
		JUMP,
		LEAVE_TRAIL,
		REDO_MAKEUP,
		LOSE_CROWN,
		TAKE_CROWN
	};

	typedef struct ClientFrame {
		int id;
		int ctr;
		MovementKey movementKey;
		vec3 cameraDirection;
		bool readyToPlay = false; // whether this client is waiting to play another match
	} ClientFrame;

	enum class AudioId {
		COLLISION = 0,
		MAKEUP,
		CROWN_CHANGE,
		NUM_AUDIO,
		NO_AUDIO = -1
	};

	typedef struct AudioTrigger {
		AudioId id;
		vec3 position;
	} AudioTrigger;

	typedef struct AnimationTrigger {
		bool makeupBooth[cse125constants::NUM_MAKEUP_STATIONS];
		bool playerCrash[cse125constants::NUM_PLAYERS];
	} AnimationTrigger;

	// Data for individual player
	typedef struct PlayerData {
		vec4 playerPosition;
		vec3 playerDirection;
		vec3 playerUp;
		RealNumber playerSpeed;
		RealNumber makeupLevel;
		RealNumber score;
		bool hasCrown;
	} PlayerData;

	// Data for loose crown
	typedef struct CrownData {
		vec3 crownPosition;
		bool crownVisible;
	} CrownData;

	typedef struct ServerFrame {
		int id; // index in players
		int ctr;
		CrownData crown;
		PlayerData players[cse125constants::NUM_PLAYERS];
		AudioTrigger audio[cse125constants::MAX_NUM_SOUNDS];
		AnimationTrigger animations;
		RealNumber gameTime;
		bool matchInProgress = true;
		int winnerId = cse125constants::DEFAULT_WINNER_ID;
		float countdownTimeRemaining = -1;
	} ServerFrame;

	const size_t CLIENT_FRAME_BUFFER_SIZE = sizeof(ClientFrame);
	const size_t SERVER_FRAME_BUFFER_SIZE = sizeof(ServerFrame);

	std::ostream& operator<<(std::ostream& os, const Action& action);
	std::ostream& operator<<(std::ostream& os, const ClientFrame* frame);
	std::ostream& operator<<(std::ostream& os, const ServerFrame* frame);

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