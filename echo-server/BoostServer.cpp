#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

const unsigned int READ_BUFFER_SIZE = 128;

std::string echoResponse(boost::array<char, READ_BUFFER_SIZE> buf, size_t len)
{
    return std::string(buf.begin(), buf.begin() + len);
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
            boost::array<char, READ_BUFFER_SIZE> buf;
            boost::system::error_code readError;
            size_t numRead = socket.read_some(boost::asio::buffer(buf), readError);

            // TODO: Update game state
            // TODO: Game logic to prepare the correct response for the client

            // Echo the response back to the server
            std::string message = echoResponse(buf, numRead);
            std::cout << "Received message from client: " << message << std::endl;

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
            std::cout << "Responded to client" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}