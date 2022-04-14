#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../Constants.hpp"
#include "../Frame.hpp"

using boost::asio::ip::tcp;

void mockGameLogic(cse125framing::Frame* frame)
{
    switch (frame->action) {
    case cse125framing::Action::LOSE_CROWN:
        frame->hasCrown = false;
        break;
    case cse125framing::Action::TAKE_CROWN:
        frame->hasCrown = true;
        break;
    default:
        break;
    }
    frame->action = cse125framing::Action::NO_ACTION;  
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
            boost::array<char, cse125constants::FRAME_BUFFER_SIZE> buffer;
            boost::system::error_code readError;
            size_t numRead = socket.read_some(boost::asio::buffer(buffer), readError);

            // Deserialize the data
            cse125framing::Frame frame;
            cse125framing::deserialize(&frame, buffer);

            std::cout << "Frame from client: " << std::endl;
            std::cout << &frame << std::endl;

            // TODO: Update game state
            // TODO: Game logic to prepare the correct response for the client
            mockGameLogic(&frame);

            // Serialize the data
            cse125framing::serialize(&frame, buffer);

            // Send a response to the client
            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(buffer), ignored_error);
            std::cout << "Responded to client" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}