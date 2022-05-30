#pragma once
#include <vector>

#include "../Definitions.hpp"
#include "../Frame.hpp"

namespace cse125gameaction {
	typedef struct GameActionContainer {
		unsigned int right : 1;
		unsigned int forward : 1;
		unsigned int left : 1;
		unsigned int backward : 1;
		unsigned int action : 1;
		vec3 cameraDirection;
	} GameActionContainer;

	GameAction gameActionFromContainer(const GameActionContainer*& gac);

	/*
	* @brief Container representing which game actions are performed in a single time step
	*/
	class GameActionTracker {
	public:
		/*
		* @brief Constructor that initializes tracker with the given number of players
		* 
		* @param the number of players to track
		*/
		GameActionTracker(unsigned int numPlayers);
		~GameActionTracker();

		/*
		* @brief Sets the player's GameActionContainer fields based on the given player id and movement key
		* 
		* @param playerId id of player
		* @param movementKey movement key sent by player
		* @param cameraDirection camera direction sent by player
		*/
		void setAction(int playerId, MovementKey movementKey, vec3 cameraDirection);

		/*
		* @brief Sets the right movement bit and clears the left movement bit for the player with the given id
		*
		* @param the id of the player to move
		*/
		void moveRight(int playerId);

		/*
		* @brief Sets the forward movement bit and clears the backward movement bit for the player with the given id
		*
		* @param the id of the player to move
		*/
		void moveForward(int playerId);

		/*
		* @brief Sets the left movement bit and clears the right movement bit for the player with the given id
		*
		* @param the id of the player to move
		*/
		void moveLeft(int playerId);

		/*
		* @brief Sets the backward movement bit and clears the forward movement bit for the player with the given id
		*
		* @param the id of the player to move
		*/
		void moveBackward(int playerId);

		/*
		* @brief Sets the action movement bit for the player with the given id
		*
		* @param the id of the player to move
		*/
		void moveAction(int playerId);

		/*
		* @brief Returns the GameActionContainer for the player with the given id
		* 
		* @return the GameActionContainer for the player with the given id
		*/
		const GameActionContainer* getGameActionContainer(int playerId);


	private:
		std::vector<GameActionContainer*> tracker;	
	};

}