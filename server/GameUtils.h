#pragma once

#include "../Config.hpp"
#include "../Constants.hpp"
#include "../Frame.hpp"
#include "../GameLogic/PhysicalObjectManager.hpp"

void initializeServerFrame(PhysicalObjectManager* manager,
                           cse125framing::ServerFrame* frame);

PhysicalObjectManager* initializeGame();

void gameLoop(PhysicalObjectManager* manager,
              int playerId,
              GameAction gameAction,
              vec3 cameraDirection);