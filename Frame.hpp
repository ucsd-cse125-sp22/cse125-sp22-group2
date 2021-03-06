#pragma once
// Represents a collection of data that is sent between the Client and Server as part
// of the core gameplay loop
#include <vector>
#include <boost/array.hpp>
#include "Definitions.hpp"
#include "Constants.hpp"

namespace cse125framing {

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
		POWERUP_PICKUP,
		POWERUP_USE,
		HONK,
		BOUNCE,
		NUM_AUDIO,
		NO_AUDIO = -1
	};

	typedef struct AudioTrigger {
		AudioId id;
		vec3 position;
	} AudioTrigger;

	typedef struct AnimationTrigger {
		bool makeupBooth[cse125constants::NUM_MAKEUP_STATIONS] = { false };
		bool playerCrash[cse125constants::NUM_PLAYERS] = { false };
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
		bool hasPowerup;
		bool powerupActive;
		float iframes;
	} PlayerData;

	// Data for loose crown
	typedef struct CrownData {
		vec3 crownPosition = vec3(0.0f);;
		bool crownVisible = false;
	} CrownData;

	// Data for powerups
	typedef struct PowerupData {
		vec3 powerupPosition = vec3(0.0f);
		bool powerupVisible = false;
	} PowerupData;

	typedef struct ServerFrame {
		int id; // index in players
		int ctr;
		CrownData crown;
		PowerupData powerup[cse125constants::NUM_POWERUPS];
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