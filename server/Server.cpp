#include <iostream>
#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"

const int NUM_CLIENTS = 1;
int frameCtr = 0;
int gameTime = 0;
int clientCtr = 0;

void initializeServerFrame(cse125framing::ServerFrame* frame) {
    frame->id = -1;
    frame->ctr = frameCtr++;
    frame->gameTime = gameTime++;
    frame->hasCrown = false;
    frame->makeupLevel = 0;
    frame->playerDirection = vec3(0.0f);
    frame->playerPosition = vec4(0.0f);
    frame->score = 0;
}

int main()
{
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
                    cse125framing::ServerFrame serverFrame;
                    initializeServerFrame(&serverFrame);
                    serverFrame.playerDirection = clientFrame.cameraDirection;

                    // Serialize the data
                    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
                    std::memcpy(&serverBuffer, &clientCtr, sizeof(int));

                    cse125framing::serialize(&serverFrame, serverBuffer);

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