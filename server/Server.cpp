#include <boost/asio.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <utility>

#include "Server.hpp"
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

GraphicsSession::GraphicsSession(
    boost::asio::ip::tcp::socket socket,
    int myid,
    std::deque<cse125framing::ClientFrame>& serverQueue,
    std::mutex& queueMtx,
    unsigned int& clientsConnected,
    bool(&clientsReplaying)[cse125constants::NUM_PLAYERS])
    : socket(std::move(socket)),
      id(myid),
      serverQueue(serverQueue),
      queueMtx(queueMtx),
      clientsConnected(clientsConnected),
      clientsReplaying(clientsReplaying)
{
    this->sessionTerminated = false;
}

void GraphicsSession::start()
{
    do_read();
}

void GraphicsSession::do_read()
{
    auto self(shared_from_this());

    socket.async_read_some(
        boost::asio::buffer(this->clientBuffer),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                cse125framing::ClientFrame clientFrame;

                // process data
                cse125framing::deserialize(&clientFrame, this->clientBuffer);

                // std::cerr << "Frame from client: " << std::endl;
                // std::cerr << &clientFrame << std::endl;

                // Check if ID needs to be sent back for initial id assignment
                if (clientFrame.id == cse125constants::DEFAULT_CLIENT_ID)
                {
                    cse125framing::ServerFrame frame;
                    frame.id = this->id;
                    do_write(&frame);
                }
                // Check if this client is indicating that it's ready to replay a match
                else if (clientFrame.replayMatch) {
                    this->clientsReplaying[this->id] = true;
                }
                else
                {
                    // write to packet buffer (queue)
                    this->queueMtx.lock();
                    this->serverQueue.push_back(clientFrame);
                    this->queueMtx.unlock();
                }

                // read more packets
                do_read();
            }
            else
            {
                // Check if the client closed the connection. If so, remember
                // this and stop future writes to this session.
                if (ec == boost::asio::error::connection_reset ||
                    ec == boost::asio::error::misc_errors::eof)
                {
                    this->sessionTerminated = true;
                }
                std::cerr << "Read failed with error " << ec << std::endl;
            }
        });
}

void GraphicsSession::do_write(cse125framing::ServerFrame* serverFrame)
{
    auto self(shared_from_this());

    cse125framing::serialize(serverFrame, this->serverBuffer);
    // std::cerr << "writing frame: " << std::endl << serverFrame << std::endl;

    boost::asio::async_write(
        socket, boost::asio::buffer(this->serverBuffer),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                // do_write will be called once per client until all clients are
                // connected
                if (this->clientsConnected < cse125constants::NUM_PLAYERS)
                {
                    this->clientsConnected++;
                    std::cerr << clientsConnected << " client(s) connected..."
                              << std::endl;
                }
            }
            else
            {
                // Happens when the client closes the game with the ESC key
                if (ec == boost::asio::error::connection_aborted)
                {
                    this->sessionTerminated = true;
                }
                std::cerr << "Write failed with error code " << ec << std::endl;
            }
        });
}

GraphicsServer::GraphicsServer(boost::asio::io_context& io_context, short port)
    : acceptor(io_context,
               boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    numConnections = 0;
    for (unsigned int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
        this->clientsReplaying[i] = false;
    }
    do_accept();
}

bool GraphicsServer::readyToReplay()
{
    for (unsigned int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
        if (!this->clientsReplaying[i]) {
            return false;
        }
    }
    return true;
}

void GraphicsServer::resetReplayStatus()
{
    for (unsigned int i = 0; i < cse125constants::NUM_PLAYERS; i++) {
        this->clientsReplaying[i] = false;
    }
}

void GraphicsServer::do_accept()
{
    // accept a connection
    acceptor.async_accept(
        [this](boost::system::error_code ec,
               boost::asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                // create new session for the connection
                std::shared_ptr<GraphicsSession> session =
                    std::make_shared<GraphicsSession>(
                        std::move(socket), this->numConnections,
                        this->serverQueue, this->queueMtx,
                        this->clientsConnected, this->clientsReplaying);
                sessions.push_back(session);
                session->start();

                this->numConnections++;
            }

            // only accept 4 connections
            if (this->numConnections < cse125constants::NUM_PLAYERS)
            {
                do_accept();
            }
        });
}

void GraphicsServer::writePackets(cse125framing::ServerFrame* serverFrame)
{
    // write to every connection
    for (std::shared_ptr<GraphicsSession>& session : sessions)
    {
        // Only write to the sessions that aren't closed
        if (!session->sessionTerminated)
        {
            session->do_write(serverFrame);
        }
    }
}