#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"
#include "../GameLogic/PhysicalObjectManager.hpp"
#include "GameAction.hpp"
#include "Server.hpp"
#include "ClockTick.hpp"

int frameCtr = 0;
int gameTime = 0;
int clientCtr = 0;

PhysicalObjectManager* manager;

boost::asio::io_context io_context;
GraphicsServer server(io_context, std::stoi(cse125constants::SERVER_PORT));


// Initializes the frame the server will send back to all clients
void initializeServerFrame(PhysicalObjectManager* manager, cse125framing::ServerFrame* frame) {
    frame->ctr = frameCtr++;
    frame->gameTime = gameTime++;

    for (int id = 0; id < cse125constants::NUM_PLAYERS; id++) {
        // TODO: Update PhysicalObjectManager to work with multiple players
        PhysicalObject* player = manager->objects->at(id);
        frame->players[id].hasCrown = false;
        frame->players[id].makeupLevel = 0;
        frame->players[id].playerDirection = player->direction;
        frame->players[id].playerPosition = vec4(player->position, 1.0f);
        frame->players[id].score = 0;
    }    
}

// Initializes and returns the PhysicalObjectManager
PhysicalObjectManager* initializeGame() {
    PhysicalObjectManager* manager = new PhysicalObjectManager();
    manager->createObject();
    return manager;
}

void gameLoop(PhysicalObjectManager* manager, int playerId, GameAction gameAction, vec3 cameraDirection) {
    PhysicalObject* player = manager->objects->at(playerId);
    switch (gameAction) {

    // Basic directions
    case GameAction::MOVE_RIGHT:
        player->moveDirection(glm::normalize(vec3(-cameraDirection.z, cameraDirection.y, cameraDirection.x)));
        break;
    case GameAction::MOVE_FORWARD:
        player->moveDirection(glm::normalize(cameraDirection));
        break;
    case GameAction::MOVE_LEFT:
        player->moveDirection(glm::normalize(vec3(cameraDirection.z, cameraDirection.y, -cameraDirection.x)));
        break;
    case GameAction::MOVE_BACKWARD:
        player->moveDirection(glm::normalize(-cameraDirection));
        break;
    
    /*
    // Compound directions
    case GameAction::MOVE_FORWARD_RIGHT:
        player->moveDirection(glm::normalize(cameraDirection));
        player->moveDirection(glm::normalize(vec3(-cameraDirection.z, cameraDirection.y, cameraDirection.x)));
        break;
    case GameAction::MOVE_FORWARD_LEFT:
        player->moveDirection(glm::normalize(cameraDirection));
        player->moveDirection(glm::normalize(vec3(cameraDirection.z, cameraDirection.y, -cameraDirection.x)));
        break;
    case GameAction::MOVE_BACKWARD_LEFT:
        player->moveDirection(glm::normalize(-cameraDirection));
        player->moveDirection(glm::normalize(vec3(cameraDirection.z, cameraDirection.y, -cameraDirection.x)));
        break;
    case GameAction::MOVE_BACKWARD_RIGHT:
        player->moveDirection(glm::normalize(-cameraDirection));
        player->moveDirection(glm::normalize(vec3(-cameraDirection.z, cameraDirection.y, cameraDirection.x)));
        break;
        */

    // Other game actions
    case GameAction::IDLE:
        // TODO: Idle behavior ?
        break;
    default:
        break;
    }
}

void launchServer(short port) {

    std::cout << "Before io_context.run()" << std::endl;
    io_context.run();
}

int main()
{
    manager = initializeGame();

    short port = 8000;
    boost::thread serverThread(launchServer, port);
    cse125clocktick::ClockTick ticker(30);

    std::cerr << "Waiting for " << cse125constants::NUM_PLAYERS << " clients to connect..." << std::endl;

    // Block until 4 clients connected
    while (server.clientsConnected < cse125constants::NUM_PLAYERS);
    std::cerr << cse125constants::NUM_PLAYERS << " clients connected!" << std::endl;

    try
    {
        while (true)
        {
            // Start the clock tick
            ticker.tickStart();
            std::deque<cse125framing::ClientFrame> serverQueue(server.serverQueue);

            // Track action information for each player
            cse125gameaction::GameActionTracker gameActionTracker(cse125constants::NUM_PLAYERS);
            std::vector<int> playerPriorities(cse125constants::NUM_PLAYERS, 0); // index i stores player i's priority
            unsigned int priorityCtr = 1;

            // Iterate from the end of where the queue currently is to the beginning
            for (auto it = serverQueue.crbegin(); it != serverQueue.crend(); it++)
            {
                const cse125framing::ClientFrame& clientFrame = *it;
                // Accumulate this player's action
                // std::cerr << "Client id is " << clientFrame.id << std::endl;

                gameActionTracker.setAction(clientFrame.id, clientFrame.movementKey, clientFrame.cameraDirection);
                // Track the priority order for this player
                if (!playerPriorities.at(clientFrame.id)) {
                    playerPriorities.at(clientFrame.id) = priorityCtr++;
                }                
            }

            int sizeBefore = serverQueue.size();
            // std::cerr << "Queue size before: " << serverQueue.size() << std::endl;
            // Empty the queue of all tasks
            server.serverQueue.clear();
            // std::cerr << "Queue size after: " << serverQueue.size() << std::endl;


            // Determine the sorted priority order
            std::vector<std::pair<int, int>> sortedPriorities; // <client id, priority> pairs
            for (size_t i = 0; i < playerPriorities.size(); i++) {
                sortedPriorities.push_back(std::make_pair(i, playerPriorities.at(i)));
            }
            // Note: Lower values indicate higher priority
            std::sort(sortedPriorities.begin(), sortedPriorities.end(), [](auto& left, auto& right) {return left.second < right.second; });

            // Update the game state in player priority order
            for (auto it = sortedPriorities.begin(); it != sortedPriorities.end(); it++) {
                const int& playerId = it->first;                
                const cse125gameaction::GameActionContainer* container = gameActionTracker.getGameActionContainer(playerId);
                GameAction gameAction = cse125gameaction::gameActionFromContainer(container);
                gameLoop(manager, playerId, gameAction, container->cameraDirection);
            }

            // TODO: Call Game loop even if no packets from any players to update score, etc. (might be a score-specific game loop)

            // Write data back to players
            cse125framing::ServerFrame serverFrame;
            initializeServerFrame(manager, &serverFrame);
            // if (sizeBefore > 0) {
                server.writePackets(&serverFrame);
            // }    

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