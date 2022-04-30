#pragma once

#include <deque>
#include <boost/asio.hpp>

#include "../Frame.hpp"

class GraphicsSession : public std::enable_shared_from_this<GraphicsSession>
{
public:
    GraphicsSession(boost::asio::ip::tcp::socket socket, int id, std::deque<cse125framing::ClientFrame>& serverQueue);

    void start();

    void do_read();
    void do_write(cse125framing::ServerFrame* serverFrame);

private:
    boost::asio::ip::tcp::socket socket;
    int id;
    std::deque<cse125framing::ClientFrame>& serverQueue;
};

class GraphicsServer
{
public:
    std::deque<cse125framing::ClientFrame> serverQueue;

    GraphicsServer(boost::asio::io_context& io_context, short port);
    void writePackets(cse125framing::ServerFrame* serverFrame);

private:
    void do_accept();

    std::vector<std::shared_ptr<GraphicsSession>> sessions;
    boost::asio::ip::tcp::acceptor acceptor;
    unsigned int numConnections;
};
