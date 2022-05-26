#include "GameUtils.h"

// Initializes the frame the server will send back to all clients
void initializeServerFrame(PhysicalObjectManager* manager,
                           cse125framing::ServerFrame* frame)
{
    static unsigned int frameCtr = 0;
    static unsigned int gameTime = 0;

    frame->ctr = frameCtr++;
    frame->gameTime = manager->gameTime;

    // initialize crown frame data
    ObjCrown* crown = (ObjCrown*)manager->objects->at(manager->crownID);
    frame->crown.crownPosition = crown->position;
    frame->crown.crownVisible = crown->loose;

    // initialize audio triggers
    for (int i = 0; i < cse125constants::MAX_NUM_SOUNDS; i++)
    {
        frame->audio[i].id = cse125framing::AudioId::NO_AUDIO;
    }

    // Index for the current sound effect
    int audioIndex = 0;

    // initialize animation triggers
    frame->animations = {}; // initialize to false --> no animations

    // initialize each player frame data
    for (int id = 0; id < cse125constants::NUM_PLAYERS; id++)
    {
        ObjPlayer* player = (ObjPlayer*)manager->objects->at(id);
        frame->players[id].hasCrown = player->hasCrown;
        frame->players[id].playerSpeed = player->speed;
        frame->players[id].makeupLevel = player->makeupLevel;
        frame->players[id].playerDirection = player->direction;
        frame->players[id].playerUp = player->up;
        frame->players[id].playerPosition = vec4(player->position, 1.0f);
        frame->players[id].score = player->score;

        // makeup booth animation + audio
        if (player->booth != -1 && player->boothTime == MAKEUP_BOOTH_TIME)
        {
            int makeupID =
                ((ObjMakeup*)manager->objects->at(player->booth))->makeupID;
            assert(makeupID < cse125constants::NUM_MAKEUP_STATIONS);
            frame->animations.makeupBooth[makeupID] = true;
            frame->audio[audioIndex].id = cse125framing::AudioId::MAKEUP;
            frame->audio[audioIndex].position = ((ObjMakeup*)manager->objects->at(makeupID))->position;
            audioIndex = (audioIndex + 1) % cse125constants::MAX_NUM_SOUNDS;
        }
        // crash animation + audio
        if (player->crashed)
        {
            frame->animations.playerCrash[id] = true;
            frame->audio[audioIndex].id = cse125framing::AudioId::COLLISION;
            frame->audio[audioIndex].position = player->position;
            audioIndex = (audioIndex + 1) % cse125constants::MAX_NUM_SOUNDS;
        }
        // crown audio
        if (player->tookCrown)
        {
            frame->audio[audioIndex].id = cse125framing::AudioId::CROWN_CHANGE;
            frame->audio[audioIndex].position = player->position;
            audioIndex = (audioIndex + 1) % cse125constants::MAX_NUM_SOUNDS;
        }
    }

    // set game restart values
    frame->matchInProgress = true;
}

void initMatchEndFrame(bool matchInProgress, int winnerId, cse125framing::ServerFrame* frame)
{
    frame->matchInProgress = matchInProgress;
    frame->winnerId = winnerId;
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
        // Idle behavior
        player->idle();
        break;
    default:
        break;
    }
}