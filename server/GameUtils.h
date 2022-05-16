#pragma once

#include "../Config.hpp"
#include "../Constants.hpp"
#include "../Frame.hpp"
#include "../GameLogic/PhysicalObjectManager.hpp"

/**
 * @brief Creates an outgoing server frame to be written to clients
 *
 * @param manager object to determine what to put in frame
 * @param frame destination of created frame
 */
void initializeServerFrame(PhysicalObjectManager* manager,
                           cse125framing::ServerFrame* frame);

/**
 * @brief Initialize a PhysicalObjectManager and begins the game
 *
 * @return PhysicalObjectManager* object used to manage game logic
 */
PhysicalObjectManager* initializeGame();

/**
 * @brief Stores players' game actions to be executed
 *
 * @param manager Pointer to manager object to store actions
 * @param playerId ID of player performing action
 * @param gameAction action of player
 * @param cameraDirection current camera direction of player
 */
void gameLoop(PhysicalObjectManager* manager,
              int playerId,
              GameAction gameAction,
              vec3 cameraDirection);