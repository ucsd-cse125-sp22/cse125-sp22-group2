#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Config.hpp"
#include "../Constants.hpp"
#include "../Frame.hpp"
#include "../GameLogic/PhysicalObjectManager.hpp"
#include "ClockTick.hpp"
#include "GameAction.hpp"
#include "Server.hpp"

int frameCtr = 0;
int gameTime = 0;
int clientCtr = 0;

PhysicalObjectManager* manager;
boost::asio::io_context io_context;

// Initializes the frame the server will send back to all clients
void initializeServerFrame(PhysicalObjectManager* manager,
                           cse125framing::ServerFrame* frame)
{
    frame->ctr = frameCtr++;
    frame->gameTime = gameTime++;

    for (int id = 0; id < cse125constants::NUM_PLAYERS; id++)
    {
        ObjPlayer* player = (ObjPlayer*)manager->objects->at(id);
        frame->players[id].hasCrown = player->hasCrown;
        frame->players[id].makeupLevel = player->makeupLevel;
        frame->players[id].playerDirection = player->direction;
        frame->players[id].playerPosition = vec4(player->position, 1.0f);
        frame->players[id].score = player->score;
    }
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

void launchServer()
{
    std::cout << "Launching Server..." << std::endl;
    io_context.run();
}

int main()
{
    // Initialize configured variables
    cse125config::initializeConfig("../config.json");

    // Initialize object manager
    manager = initializeGame();

    // Initialize network server
    GraphicsServer server(io_context, std::stoi(cse125config::SERVER_PORT));
    // Launch server communication in separate thread
    boost::thread serverThread(launchServer);

    // Initialize ticker
    cse125clocktick::ClockTick ticker(cse125config::TICK_RATE);

    // Block until 4 clients connected
    std::cerr << "Waiting for " << cse125constants::NUM_PLAYERS
              << " clients to connect..." << std::endl;

    while (server.clientsConnected < cse125constants::NUM_PLAYERS)
    {
        // idle wait for clients
    }

    std::cerr << server.clientsConnected << " clients connected!" << std::endl;

    // server loop
    try
    {
        while (true)
        {
            // Start the clock tick
            ticker.tickStart();

            const std::deque<cse125framing::ClientFrame> serverQueue(
                server.serverQueue);

            // Track action information for each player
            cse125gameaction::GameActionTracker gameActionTracker(
                cse125constants::NUM_PLAYERS);

            // index i stores player i's priority
            std::vector<int> playerPriorities(cse125constants::NUM_PLAYERS, 0);

            unsigned int priorityCtr = 1;

            // Iterate from the end of where the queue currently is to the
            // beginning
            for (auto it = serverQueue.crbegin(); it != serverQueue.crend();
                 it++)
            {
                const cse125framing::ClientFrame& clientFrame = *it;

                gameActionTracker.setAction(clientFrame.id,
                                            clientFrame.movementKey,
                                            clientFrame.cameraDirection);

                // Track the priority order for this player
                if (!playerPriorities.at(clientFrame.id))
                {
                    // Note: Lower values indicate higher priority
                    playerPriorities.at(clientFrame.id) = priorityCtr++;
                }
            }

            // Empty the queue of all tasks
            server.queueMtx.lock();
            server.serverQueue.clear();
            server.queueMtx.unlock();

            // Determine the sorted priority order
            // <priority, client_id> pairs
            std::map<int, int> sortedPriorities;
            for (size_t i = 0; i < playerPriorities.size(); i++)
            {
                sortedPriorities[playerPriorities[i]] = i;
            }

            // Update basic game state (not dependent on input)
            manager->step();
            // Update the game state in player priority order
            for (auto it = sortedPriorities.begin();
                 it != sortedPriorities.end(); it++)
            {
                const int& playerId = it->second;

                const cse125gameaction::GameActionContainer* container =
                    gameActionTracker.getGameActionContainer(playerId);

                GameAction gameAction =
                    cse125gameaction::gameActionFromContainer(container);

                gameLoop(manager, playerId, gameAction,
                         container->cameraDirection);
            }

            // TODO: Call Game loop even if no packets from any players to
            // update score, etc. (might be a score-specific game loop)

            // Write data back to players
            cse125framing::ServerFrame serverFrame;
            initializeServerFrame(manager, &serverFrame);
            server.writePackets(&serverFrame);

            // Sleep until the end of the clock tick
            ticker.tickEnd();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    // Wait for the server thread to finish
    serverThread.join();

    return 0;
}