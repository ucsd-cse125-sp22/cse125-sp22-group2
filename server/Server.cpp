#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"

using boost::asio::ip::tcp;

int frameCtr = 0;
int gameTime = 0;

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

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

        std::cout << "About to enter main server loop" << std::endl;
        for (;;)
        {
            // Listen for and accept incoming connections
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::cout << "Accepted a connection" << std::endl;

            // Read the data from a client
            boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
            boost::system::error_code readError;
            size_t numRead = socket.read_some(boost::asio::buffer(clientBuffer), readError);

            // Deserialize the data
            cse125framing::ClientFrame clientFrame;
            cse125framing::deserialize(&clientFrame, clientBuffer);

            std::cout << "Frame from client: " << std::endl;
            std::cout << &clientFrame << std::endl;

            // TODO: Update game state
            // TODO: Game logic to prepare the correct response for the client
            cse125framing::ServerFrame serverFrame;
            initializeServerFrame(&serverFrame);

            // Serialize the data
            boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
            cse125framing::serialize(&serverFrame, serverBuffer);

            // Send a response to the client
            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(serverBuffer), ignored_error);
            std::cout << "Responded to client" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}