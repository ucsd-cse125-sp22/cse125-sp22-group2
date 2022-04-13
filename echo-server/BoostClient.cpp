#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string getUserInput() {
    // TODO: Get input from I/O device (e.g. keyboard) instead of hard-coded message
    std::cout << "Enter data to send to the server" << std::endl;
    std::string userInput;
    std::cin >> userInput;
    return userInput;
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

            // Get user input from game
            std::string userInput = getUserInput();
            // TODO: Convert user input to an Event
            // Send event information to server
            std::cout << "About to send info to server" << std::endl;
            boost::system::error_code writeError;
            size_t numWritten = boost::asio::write(socket, boost::asio::buffer(userInput), writeError);
            // TODO: Handle writeError
            std::cout << "Sent info to server, now waiting for response" << std::endl;

            // Wait for server response
            boost::array<char, 128> buf;
            boost::system::error_code error;
            size_t numRead = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof) {
                std::cout << "EOF from server:" << std::endl;
                break; // Connection closed cleanly by peer.
            }
            else if (error) {
                throw boost::system::system_error(error); // Some other error.
            }
            
            std::cout << "Response from server:" << std::endl;
            std::string response = std::string(buf.begin(), buf.begin() + numRead);
            std::cout.write(buf.data(), numRead);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}