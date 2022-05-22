#include "GameUtils.h"

// Initializes the frame the server will send back to all clients
void initializeServerFrame(PhysicalObjectManager* manager,
                           cse125framing::ServerFrame* frame)
{
    static unsigned int frameCtr = 0;
    static unsigned int gameTime = 0;

    frame->ctr = frameCtr++;
    frame->gameTime = gameTime++;

    // initialize audio triggers
    for (int i = 0; i < cse125constants::MAX_NUM_SOUNDS; i++)
    {
        frame->audio[i].id = cse125framing::AudioId::NO_AUDIO;
    }

    // initialize animation triggers
    frame->animations = {}; // initialize to false --> no animations

    // initialize each player frame data
    for (int id = 0; id < cse125constants::NUM_PLAYERS; id++)
    {
        ObjPlayer* player = (ObjPlayer*)manager->objects->at(id);
        frame->players[id].hasCrown = player->hasCrown;
        frame->players[id].makeupLevel = player->makeupLevel;
        frame->players[id].playerDirection = player->direction;
        frame->players[id].playerPosition = vec4(player->position, 1.0f);
        frame->players[id].score = player->score;
    }

    // set game restart values
    frame->matchInProgress = true;
}

// Initializes and returns the PhysicalObjectManager
PhysicalObjectManager* initializeGame()
{
    PhysicalObjectManager* manager = new PhysicalObjectManager();
    manager->startGame();
    return manager;
}

void gameLoop(PhysicalObjectManager* manager,
              int playerId,
              GameAction gameAction,
              vec3 cameraDirection)
{
    ObjPlayer* player = (ObjPlayer*)manager->objects->at(playerId);
    switch (gameAction)
    {

    // Basic directions
    case GameAction::MOVE_RIGHT:
        player->action(glm::normalize(
            vec3(-cameraDirection.z, cameraDirection.y, cameraDirection.x)));
        break;
    case GameAction::MOVE_FORWARD:
        player->action(glm::normalize(cameraDirection));
        break;
    case GameAction::MOVE_LEFT:
        player->action(glm::normalize(
            vec3(cameraDirection.z, cameraDirection.y, -cameraDirection.x)));
        break;
    case GameAction::MOVE_BACKWARD:
        player->action(glm::normalize(-cameraDirection));
        break;

    // Compound directions
    case GameAction::MOVE_FORWARD_RIGHT:
        player->action(glm::normalize(
            vec3(-cameraDirection.z, cameraDirection.y, cameraDirection.x) +
            cameraDirection));
        break;
    case GameAction::MOVE_FORWARD_LEFT:
        player->action(glm::normalize(
            vec3(cameraDirection.z, cameraDirection.y, -cameraDirection.x) +
            cameraDirection));
        break;
    case GameAction::MOVE_BACKWARD_LEFT:
        player->action(glm::normalize(
            vec3(cameraDirection.z, cameraDirection.y, -cameraDirection.x) -
            cameraDirection));
        break;
    case GameAction::MOVE_BACKWARD_RIGHT:
        player->action(glm::normalize(
            vec3(-cameraDirection.z, cameraDirection.y, cameraDirection.x) -
            cameraDirection));
        break;

    // Other game actions
    case GameAction::IDLE:
        // TODO: Idle behavior ?
        break;
    default:
        break;
    }
}