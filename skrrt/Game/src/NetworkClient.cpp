#include <Debug.h>
#include "NetworkClient.hpp"

cse125networkclient::NetworkClient::NetworkClient(std::string host, std::string port)
{
    this->clientId = cse125constants::DEFAULT_CLIENT_ID;
    this->resolver = std::make_unique<boost::asio::ip::tcp::resolver>(this->context);
    this->endpoints = this->resolver->resolve(host, port);
    this->socket = std::make_unique<boost::asio::ip::tcp::socket>(this->context);
    boost::asio::connect(*this->socket, this->endpoints);
}

cse125networkclient::NetworkClient::~NetworkClient()
{
}

int cse125networkclient::NetworkClient::getId()
{
    // Prepare a frame to send to the server
    cse125framing::ClientFrame frame;
    frame.id = cse125constants::DEFAULT_CLIENT_ID;

    // Until the id request is successful, 
    // attempt to get this client's id from the server
    int clientId = cse125constants::DEFAULT_CLIENT_ID;
    while (clientId == cse125constants::DEFAULT_CLIENT_ID)
    {
        // Send a frame with an id of -1 to indicate the the client
        // is requesting an id, as opposed to just sending a regular packet
        boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
        cse125framing::serialize(&frame, clientBuffer);

        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "sending frame: \n" << &frame << std::endl;
        }

        boost::system::error_code writeError;
        // Send the packet to the server
        size_t numWritten = boost::asio::write(*this->socket, boost::asio::buffer(clientBuffer), writeError);
        // If the write was unsuccessful, retry
        if (writeError) {
            if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
                std::cerr << "Error contacting server, retrying ..." << std::endl;
            }
            continue;
        }

        // Get the response from the server
        boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
        boost::system::error_code readError;
        size_t numRead = boost::asio::read(*this->socket, boost::asio::buffer(serverBuffer), readError);

        // If server closed connection
        if (readError == boost::asio::error::eof) {
            if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
                std::cout << "EOF from server:" << std::endl; 
            }
            break;
        }
        // TODO: Handle fatal error from server
        else if (readError)
        {
            throw boost::system::system_error(readError); // Some other error.
        }
        // Successful read
        else
        {
            // Parse the id provided by the server
            cse125framing::ServerFrame frame;
            cse125framing::deserialize(&frame, serverBuffer);
            this->clientId = frame.id;
            if (DEBUG_LEVEL >= LOG_LEVEL_INFO) {
                std::cout << "Client id is now " << this->clientId << std::endl;
            }
            break;
        }
    }
	return this->clientId;
}

size_t cse125networkclient::NetworkClient::send(MovementKey movementKey, vec3 cameraDirection, boost::system::error_code* errorCode)
{
    // Prepare frame to send to server
    cse125framing::ClientFrame frame;
    frame.id = this->clientId;
    frame.ctr = this->clientFrameCtr++;
    frame.movementKey = movementKey;
    frame.cameraDirection = glm::vec3(cameraDirection);

    // Serialize frame
    boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
    cse125framing::serialize(&frame, clientBuffer);

    // Send frame to server
    boost::system::error_code error;
    size_t numWritten = boost::asio::write(*this->socket, boost::asio::buffer(clientBuffer), error);
    if (error) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error sending packet to server, continuing ..." << std::endl;
            std::cerr << error << std::endl;
        }
    }
    // Set error code
    *errorCode = error;
    return numWritten;
}

size_t cse125networkclient::NetworkClient::receive(cse125framing::ServerFrame* frame, boost::system::error_code* errorCode)
{
    // Get a response from the server, blocking until all the data are read
    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
    boost::system::error_code error;
    size_t numRead = boost::asio::read(*this->socket, boost::asio::buffer(serverBuffer), error);

    if (error == boost::asio::error::eof) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cout << "EOF from server." << std::endl; // Server closed connection
        }
    }
    else if (error) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error reading from server: " << error << std::endl; // Some other error.
        }
    }
    else
    {
        // Deserialize the server response
        cse125framing::deserialize(frame, serverBuffer);

        // Assert that we read all the bytes we expected to
        assert(numRead == cse125framing::SERVER_FRAME_BUFFER_SIZE);
        if (DEBUG_LEVEL >= LOG_LEVEL_FINE) {
            std::cout << "Received reply from server." << std::endl;
            std::cout << numRead << " " << sizeof(cse125framing::ServerFrame) << std::endl;
            std::cout << frame << std::endl;
        }        
    }
    // Set error code
    *errorCode = error;
    return size_t();
}

void cse125networkclient::NetworkClient::closeConnection()
{
    boost::system::error_code errorCode;
    this->socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
    if (errorCode) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error shutting down socket" << std::endl;
        }
    }
    this->socket->close(errorCode);
    if (errorCode) {
        if (DEBUG_LEVEL >= LOG_LEVEL_ERROR) {
            std::cerr << "Error closing socket" << std::endl;
        }
    }
}
