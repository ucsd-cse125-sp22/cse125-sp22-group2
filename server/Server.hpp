#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <mutex>

#include "../Frame.hpp"

class GraphicsSession : public std::enable_shared_from_this<GraphicsSession>
{
  public:
    /**
     * @brief boolean to determine if session has been closed
     */
    bool sessionTerminated;

    /**
     * @brief Construct a new Graphics Session object
     *
     * @param socket boost socket in charge of network communication
     * @param id client id number to index client's data
     * @param serverQueue reference to server's frame queue to add frames to
     * @param queueMtx mutex variable to ensure syncronous writes
     * @param clientsConnected tells server when another client is connected
     */
    GraphicsSession(boost::asio::ip::tcp::socket socket,
                    int id,
                    std::deque<cse125framing::ClientFrame>& serverQueue,
                    std::mutex& queueMtx,
                    unsigned int& clientsConnected);

    /**
     * @brief Begin the session by reading for a packet
     */
    void start();

    /**
     * @brief Read and process an incoming packet
     */
    void do_read();
    /**
     * @brief Write a packet to the server
     *
     * @param serverFrame Frame to write to the server
     */
    void do_write(cse125framing::ServerFrame* serverFrame);

  private:
    boost::asio::ip::tcp::socket socket;
    int id;
    unsigned int& clientsConnected;
    std::deque<cse125framing::ClientFrame>& serverQueue;
    std::mutex& queueMtx;
    boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;
    boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;
};

class GraphicsServer
{
  public:
    /**
     * @brief Queue to hold frames written to the server
     */
    std::deque<cse125framing::ClientFrame> serverQueue;
    /**
     * @brief mutex variable associated with serverQueue
     */
    std::mutex queueMtx;
    /**
     * @brief number of clients connected to the server
     */
    unsigned int clientsConnected;

    /**
     * @brief Construct a new Graphics Server object
     *
     * @param io_context io context of the server
     * @param port port to connect to
     */
    GraphicsServer(boost::asio::io_context& io_context, short port);
    /**
     * @brief Writes a server frame to all of the connected clients
     *
     * @param serverFrame frame to write to clients
     */
    void writePackets(cse125framing::ServerFrame* serverFrame);

  private:
    void do_accept();

    std::vector<std::shared_ptr<GraphicsSession>> sessions;
    boost::asio::ip::tcp::acceptor acceptor;
    unsigned int numConnections;
};
