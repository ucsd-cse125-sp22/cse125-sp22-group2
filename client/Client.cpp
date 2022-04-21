#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"

using boost::asio::ip::tcp;

int frameCtr = 0;
int frameId = -1;

void getUserInput(cse125framing::ClientFrame* frame) {
    // TODO: Get input from I/O device (e.g. keyboard) instead of hard-coded message
    frame->cameraDirection = vec3(1, 2, 3);
    frame->ctr = frameCtr++;
    frame->id = frameId;
    frame->movementKey = cse125framing::MovementKey::FORWARD;
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
            resolver.resolve(argv[1], "13");

        while (true) {
            // Initiate a connection to the server
            // TODO: Persistent connection optimization
            std::cout << "Connected to server" << std::endl;
            tcp::socket socket(io_context);
            boost::asio::connect(socket, endpoints);

            // Get user input from game and set the fields of the Frame
            cse125framing::ClientFrame clientFrame;
            getUserInput(&clientFrame);

            // Serialize the Frame
            boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
            cse125framing::serialize(&clientFrame, clientBuffer);
 
            std::cout << "About to send info to server" << std::endl;
            boost::system::error_code writeError;
            size_t numWritten = boost::asio::write(socket, boost::asio::buffer(clientBuffer), writeError);
            // TODO: Handle writeError
            std::cout << "Sent info to server, now waiting for response" << std::endl;

            // Get server response
            boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
            boost::system::error_code error;
            size_t numRead = socket.read_some(boost::asio::buffer(serverBuffer), error);

            if (error == boost::asio::error::eof) {
                std::cout << "EOF from server:" << std::endl;
                break; // Connection closed cleanly by peer.
            }
            else if (error) {
                throw boost::system::system_error(error); // Some other error.
            }

            // Deserialize the data
            cse125framing::ServerFrame serverFrame;
            cse125framing::deserialize(&serverFrame, serverBuffer);

            std::cout << "Frame from server: " << std::endl;
            std::cout << &serverFrame << std::endl;

        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}