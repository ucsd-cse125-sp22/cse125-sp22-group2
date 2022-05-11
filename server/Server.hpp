#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <mutex>

#include "../Frame.hpp"

class GraphicsSession : public std::enable_shared_from_this<GraphicsSession>
{
  public:
    bool sessionTerminated;
    GraphicsSession(boost::asio::ip::tcp::socket socket,
                    int id,
                    std::deque<cse125framing::ClientFrame>& serverQueue,
                    std::mutex& queueMtx,
                    unsigned int& clientsConnected);

    void start();

    void do_read();
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
    std::deque<cse125framing::ClientFrame> serverQueue;
    std::mutex queueMtx;
    unsigned int clientsConnected;

    GraphicsServer(boost::asio::io_context& io_context, short port);
    void writePackets(cse125framing::ServerFrame* serverFrame);

  private:
    void do_accept();

    std::vector<std::shared_ptr<GraphicsSession>> sessions;
    boost::asio::ip::tcp::acceptor acceptor;
    unsigned int numConnections;
};
