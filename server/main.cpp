#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <map>
#include <string>
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
    GraphicsServer* server =
        new GraphicsServer(io_context, std::stoi(cse125config::SERVER_PORT));
    boost::thread serverThread(launchServer);

    // Initialize ticker
    cse125clocktick::ClockTick ticker(cse125config::TICK_RATE);

    // Block until all clients connected
    std::cout << "Waiting for " << cse125constants::NUM_PLAYERS
              << " clients to connect..." << std::endl;

    while (server->clientsConnected < cse125constants::NUM_PLAYERS)
    {
        // idle wait for clients
    }

    // Wait for all clients to be ready to start playing
    std::cerr << "Waiting for clients to start playing..." << std::endl;
    server->setReadyToPlay(false);
    while (!server->readyToPlay()) {} // Idle wait
    std::cerr << "All clients ready to start playing! " << std::endl;

    const int numCountdownTicks = cse125config::TICK_RATE * cse125config::COUNTDOWN_LENGTH;
    // server loop
    manager = initializeGame();
    bool runServer = true; 
    while (runServer) 
    {
        if (cse125config::ENABLE_COUNTDOWN) {
            // Pre-match countdown loop
            for (int i = 0; i <= numCountdownTicks; i++) {
                ticker.tickStart();
                cse125framing::ServerFrame countdownFrame;
                initializeServerFrame(manager, &countdownFrame);
                countdownFrame.countdownTimeRemaining = numCountdownTicks - i;
                server->writePackets(&countdownFrame);
                ticker.tickEnd();
            }
        }      

        // State about the current match
        bool matchInProgress = true;
        int winnerId = cse125constants::DEFAULT_WINNER_ID;
        std::cerr << "Starting Skrrt Skirt!" << std::endl;
        while (matchInProgress)
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

            // Ensure all players have a priority, even if 
            // they didn't send a packet this server tick
            for (int i = 0; i < playerPriorities.size(); i++) {
                if (!playerPriorities.at(i)) {
                    playerPriorities.at(i) = priorityCtr++;
                }
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

            // Update basic game state (score, makeup levels; not dependent on input)
            manager->step(&matchInProgress, &winnerId);

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

            // Write data back to players
            cse125framing::ServerFrame serverFrame;
            initializeServerFrame(manager, &serverFrame);
            server->writePackets(&serverFrame);
            // Sleep until the end of the clock tick
            ticker.tickEnd();
        } 

        std::cerr << "Match has ended!" << std::endl;

        // Reset the game manager
        delete manager;

        // Tell clients that the match finished
        cse125framing::ServerFrame matchEndFrame;
        initMatchEndFrame(matchInProgress, winnerId, &matchEndFrame);
        server->writePackets(&matchEndFrame);

        // Wait for all clients to be ready to play again
        std::cerr << "Waiting for clients to replay..." << std::endl;
        server->setReadyToPlay(false);
        while (!server->readyToPlay()) {}

        // Reset the server packet queue
        server->queueMtx.lock();
        server->serverQueue.clear();
        server->queueMtx.unlock();

        // Re-initialize the manager so that the starting positions of players are set
        manager = initializeGame();

        // All clients are ready: notify clients that the game has restarted
        cse125framing::ServerFrame matchRestartedFrame;
        initializeServerFrame(manager, &matchRestartedFrame);
        matchRestartedFrame.countdownTimeRemaining = numCountdownTicks;

        std::cerr << "All clients ready to restart! Notifying clients..." << std::endl;
        server->writePackets(&matchRestartedFrame);
    }  

    // Wait for the server thread to finish
    serverThread.join();

    return 0;
}