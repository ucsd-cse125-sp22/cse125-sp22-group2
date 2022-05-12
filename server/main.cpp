#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ClockTick.hpp"
#include "GameAction.hpp"
#include "GameUtils.h"
#include "Server.hpp"

PhysicalObjectManager* manager;
boost::asio::io_context io_context;

void launchServer()
{
    std::cout << "Launching Server..." << std::endl;
    io_context.run();
}

int main()
{
    // Initialize server and game
    cse125config::initializeConfig("../config.json");
    manager = initializeGame();
    GraphicsServer* server =
        new GraphicsServer(io_context, std::stoi(cse125config::SERVER_PORT));
    boost::thread serverThread(launchServer);

    // Initialize ticker
    cse125clocktick::ClockTick ticker(cse125config::TICK_RATE);

    // Block until 4 clients connected
    std::cout << "Waiting for " << cse125constants::NUM_PLAYERS
              << " clients to connect..." << std::endl;

    while (server->clientsConnected < cse125constants::NUM_PLAYERS)
    {
        // idle wait for clients
    }

    std::cout << "Starting Skrrt Skirt!" << std::endl;

    // server loop
    try
    {
        while (true)
        {
            // Start the clock tick
            ticker.tickStart();

            const std::deque<cse125framing::ClientFrame> serverQueue(
                server->serverQueue);

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
                // Note: Higher values indicate higher priority
                playerPriorities.at(clientFrame.id) = priorityCtr++;
            }

            // Empty the queue of all tasks
            server->queueMtx.lock();
            server->serverQueue.clear();
            server->queueMtx.unlock();

            // Determine the sorted priority order
            // <priority, client_id> pairs
            std::map<int, int, std::greater<int>> sortedPriorities;
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
            server->writePackets(&serverFrame);

            // Sleep until the end of the clock tick
            ticker.tickEnd();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    delete manager;
    // Wait for the server thread to finish
    serverThread.join();

    return 0;
}