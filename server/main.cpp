#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"
#include "../GameLogic/PhysicalObjectManager.hpp"
#include "Server.hpp"
#include "ClockTick.hpp"

const int NUM_CLIENTS = 1;
int frameCtr = 0;
int gameTime = 0;
int clientCtr = 0;

PhysicalObjectManager* manager;

boost::asio::io_context io_context;
GraphicsServer server(io_context, std::stoi(cse125constants::SERVER_PORT));

void initializeServerFrame(PhysicalObjectManager* manager, int id, cse125framing::ServerFrame* frame) {
    frame->id = id;
    PhysicalObject* player = manager->objects->at(id);
    frame->ctr = frameCtr++;
    frame->gameTime = gameTime++;
    frame->players[id].hasCrown = false;
    frame->players[id].makeupLevel = 0;
    frame->players[id].playerDirection = player->direction;
    frame->players[id].playerPosition = vec4(player->position, 1.0f);
    frame->players[id].score = 0;
}

PhysicalObjectManager* initializeGame() {
    PhysicalObjectManager* manager = new PhysicalObjectManager();
    manager->createObject();
    return manager;
}

void gameLoop(PhysicalObjectManager* manager, int clientID, cse125framing::MovementKey movementKey, vec3 cameraDirection) {
    PhysicalObject* player = manager->objects->at(clientID);
    switch (movementKey) {
    case cse125framing::MovementKey::RIGHT:
        player->moveDirection(glm::normalize(vec3(-cameraDirection.z, cameraDirection.y, cameraDirection.x)));
        break;
    case cse125framing::MovementKey::FORWARD:
        player->moveDirection(glm::normalize(cameraDirection));
        break;
    case cse125framing::MovementKey::LEFT:
        player->moveDirection(glm::normalize(vec3(cameraDirection.z, cameraDirection.y, -cameraDirection.x)));
        break;
    case cse125framing::MovementKey::BACKWARD:
        player->moveDirection(glm::normalize(-cameraDirection));
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
    cse125clocktick::ClockTick ticker(1);

    std::cerr << "Waiting for " << cse125constants::NUM_PLAYERS << " clients to connect..." << std::endl;

    // Block until 4 clients connected
    while (server.clientsConnected < cse125constants::NUM_PLAYERS);
    std::cerr << cse125constants::NUM_PLAYERS << " clients connected!" << std::endl;

    try
    {
        while (true)
        {
            ticker.tickStart();
            std::deque<cse125framing::ClientFrame>& serverQueue = server.serverQueue;
            std::deque<cse125framing::ClientFrame> processQueue;
            std::unordered_set<int> usedIds;

            // for i to queue size
            for (auto it = serverQueue.crbegin(); it != serverQueue.crend(); it++)
            {
                // check if id is already added
                if (usedIds.count(it->id) == 0)
                {
                    // basic functionality for priority: only take last packet
                    processQueue.push_front(*it); 
                    usedIds.insert(it->id);
                }
            }
            serverQueue.clear();

            // deque, process frame, call physic logic
            while (processQueue.size() > 0)
            {
                cse125framing::ClientFrame clientFrame = processQueue.front();
			    // gameLoop(manager, clientFrame.id, clientFrame.movementKey, clientFrame.cameraDirection);
                processQueue.pop_front();

            }


            // update game state


            // write packet to clients
            cse125framing::ServerFrame serverFrame;
            serverFrame.ctr = frameCtr++;

            //std::cerr << "Sending packet with ctr: " << serverFrame.ctr << std::endl;
            server.writePackets(&serverFrame);

            ticker.tickEnd();
            /*
            
			// TODO: Update game state
			// TODO: Game logic to prepare the correct response for the client
			cse125framing::ServerFrame serverFrame;
			// initializeServerFrame(manager, clientFrame.id, &serverFrame);

			// Serialize the data
			boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
			std::memcpy(&serverBuffer, &clientCtr, sizeof(cse125framing::ServerFrame));

			cse125framing::serialize(&serverFrame, serverBuffer);


			cout << &serverFrame << endl;
			cse125framing::deserialize(&serverFrame, serverBuffer);
			cout << &serverFrame << endl;

			// Send a response to the client
			boost::system::error_code ignored_error;
			boost::asio::write(socket, boost::asio::buffer(serverBuffer), ignored_error);
			std::cout << "Responded to client" << std::endl;
            */
        }
    
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    serverThread.join();

    return 0;
}