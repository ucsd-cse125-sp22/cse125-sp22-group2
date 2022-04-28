#pragma once

#include <deque>
#include <boost/asio.hpp>

#include "../Frame.hpp"

class GraphicsSession : public std::enable_shared_from_this<GraphicsSession>
{
public:
    GraphicsSession(boost::asio::ip::tcp::socket socket, int id, std::deque<cse125framing::ClientFrame>& serverQueue);

    void start();

private:
    void do_read();

    void do_write(cse125framing::ServerFrame* serverFrame);

    boost::asio::ip::tcp::socket socket;
    int id;
    std::deque<cse125framing::ClientFrame>& serverQueue;
};

class GraphicsServer
{
public:
    GraphicsServer(boost::asio::io_context& io_context, short port);
    std::deque<cse125framing::ClientFrame> serverQueue;

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor;
    unsigned int numConnections;
};
