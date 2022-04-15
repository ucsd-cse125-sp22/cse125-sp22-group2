#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"

using boost::asio::ip::tcp;

void getUserInput(cse125framing::Frame* frame) {
    // TODO: Get input from I/O device (e.g. keyboard) instead of hard-coded message
    frame->playerPosition = vec4(1, 2, 3, 4);
    frame->movementDirection = vec3(5, 6, 7);
    frame->makeupLevel = 15;
    frame->score = 20;
    frame->hasCrown = true;
    frame->action = cse125framing::Action::LOSE_CROWN;    
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
            resolver.resolve(argv[1], "daytime");

        while (true) {
            // Initiate a connection to the server
            // TODO: Persistent connection optimization
            std::cout << "Connected to server" << std::endl;
            tcp::socket socket(io_context);
            boost::asio::connect(socket, endpoints);

            // Get user input from game and set the fields of the Frame
            cse125framing::Frame frame;
            getUserInput(&frame);

            // Serialize the Frame
            boost::array<char, cse125constants::FRAME_BUFFER_SIZE> buffer;
            cse125framing::serialize(&frame, buffer);
 
            std::cout << "About to send info to server" << std::endl;
            boost::system::error_code writeError;
            size_t numWritten = boost::asio::write(socket, boost::asio::buffer(buffer), writeError);
            // TODO: Handle writeError
            std::cout << "Sent info to server, now waiting for response" << std::endl;

            // Get server response
            boost::system::error_code error;
            size_t numRead = socket.read_some(boost::asio::buffer(buffer), error);

            if (error == boost::asio::error::eof) {
                std::cout << "EOF from server:" << std::endl;
                break; // Connection closed cleanly by peer.
            }
            else if (error) {
                throw boost::system::system_error(error); // Some other error.
            }

            // Deserialize the data
            cse125framing::deserialize(&frame, buffer);

            std::cout << "Frame from server: " << std::endl;
            std::cout << &frame << std::endl;

        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}