#include <iostream>
#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"
#include "../GameLogic/PhysicalObjectManager.hpp"

const int NUM_CLIENTS = 1;
int frameCtr = 0;
int gameTime = 0;
int clientCtr = 0;

PhysicalObjectManager* manager;

void initializeServerFrame(PhysicalObjectManager* manager, int id, cse125framing::ServerFrame* frame) {
    frame->id = id;
    PhysicalObject* player = manager->objects->at(id);
    frame->ctr = frameCtr++;
    frame->gameTime = gameTime++;
    frame->hasCrown = false;
    frame->makeupLevel = 0;
    frame->playerDirection = player->direction;
    frame->playerPosition = vec4(player->position, 1.0f);
    frame->score = 0;
}

PhysicalObjectManager* initializeGame() {
    PhysicalObjectManager* manager = new PhysicalObjectManager();
    manager->createObject();
    return manager;
}

void gameLoop(PhysicalObjectManager* manager, int clientID, cse125framing::MovementKey movementKey, vec3 cameraDirection) {
    ObjPlayer* player = (ObjPlayer*) manager->objects->at(clientID);
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

int main()
{
    manager = initializeGame();

    try
    {
        boost::asio::io_context io_context;

        boost::asio::ip::tcp::acceptor acceptor(io_context,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(cse125constants::SERVER_PORT)));

        int numClientsRegistered = 0;
        while (true) {
            // Accept connections
            boost::asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::cout << "Accepted a connection" << std::endl;

            while (true) {
                // Read the data from a client
                boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
                boost::system::error_code readError;
                size_t numRead = socket.read_some(boost::asio::buffer(clientBuffer), readError);

                if (readError == boost::asio::error::eof) {
                    std::cout << "Client closed connection" << std::endl;
                    break;
                }
                else if (readError) {
                    std::cout << "Error reading from client: " << readError << std::endl;
                    if (readError == boost::system::errc::connection_reset) {
                        break;
                    }
                    continue;
                }

                // Deserialize the data
                cse125framing::ClientFrame clientFrame;
                cse125framing::deserialize(&clientFrame, clientBuffer);

                std::cout << "Frame from client: " << std::endl;
                std::cout << &clientFrame << std::endl;

                // Check if the client is requesting an id
                if (clientFrame.id == cse125constants::DEFAULT_CLIENT_ID) {
                    // Send an int id
                    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
                    std::memcpy(&serverBuffer, &clientCtr, sizeof(int));
                    boost::system::error_code writeError;
                    boost::asio::write(socket, boost::asio::buffer(serverBuffer), writeError);
                    if (writeError) {
                        std::cerr << "Error sending clientId " << clientCtr << " to client, not incrementing clientCtr" << std::endl;
                    }
                    else {
                        numClientsRegistered += 1;
                        std::cout << "Registered client " << numClientsRegistered << std::endl;
                    }
                }
                else {
                    // TODO: Update game state
                    // TODO: Game logic to prepare the correct response for the client
                    gameLoop(manager, clientFrame.id, clientFrame.movementKey, clientFrame.cameraDirection);
                    cse125framing::ServerFrame serverFrame;
                    initializeServerFrame(manager, clientFrame.id, &serverFrame);

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
                }
            }                 
         }          
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}