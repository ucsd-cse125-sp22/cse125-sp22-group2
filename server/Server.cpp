#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <deque>
#include <boost/asio.hpp>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "Server.hpp"


GraphicsSession::GraphicsSession(boost::asio::ip::tcp::socket socket, int myid, std::deque<cse125framing::ClientFrame>& serverQueue, std::mutex& queueMtx, unsigned int& clientsConnected)
	: socket(std::move(socket)), id(myid), serverQueue(serverQueue), queueMtx(queueMtx), clientsConnected(clientsConnected)
{
}

void GraphicsSession::start()
{
	do_read();
}

void GraphicsSession::do_read()
{
	std::cerr << "do_read()\n";
	auto self(shared_from_this());

	socket.async_read_some(boost::asio::buffer(this->clientBuffer), 
		[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				cse125framing::ClientFrame clientFrame;

				// process data
				cse125framing::deserialize(&clientFrame, this->clientBuffer);

				// std::cerr << "Frame from client: " << std::endl;
				// std::cerr << &clientFrame << std::endl;

				// Check if ID needs to be sent back
				if (clientFrame.id == cse125constants::DEFAULT_CLIENT_ID)
				{
					// lock
					// std::cerr << "Giving client id: " << this->id << std::endl;
					cse125framing::ServerFrame frame;
					frame.id = this->id;
					do_write(&frame);
					// free lock
				}
				else
				{
					// write to packet buffer (queue)
					this->queueMtx.lock();
					this->serverQueue.push_back(clientFrame);
					this->queueMtx.unlock();
					//std::cerr << "Server queue size: " << this->serverQueue.size() << std::endl;
				}

				// read more packets
				do_read();
			}
		});
}

void GraphicsSession::do_write(cse125framing::ServerFrame* serverFrame)
{
	auto self(shared_from_this());

	cse125framing::serialize(serverFrame, this->serverBuffer); 
	std::cerr << "writing frame: " << std::endl << serverFrame << std::endl;

	boost::asio::async_write(socket, boost::asio::buffer(this->serverBuffer),
		[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{				
				if (this->clientsConnected < cse125constants::NUM_PLAYERS) {
					this->clientsConnected++;
					std::cerr << "Clients connected: " << this->clientsConnected << std::endl;
				}
			}
			else
			{
				std::cerr << "Write failed.. NOT rewriting" << std::endl;
				// do_write(serverFrame);
			}
		});
}

GraphicsServer::GraphicsServer(boost::asio::io_context& io_context, short port)
	: acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
	numConnections = 0;
	do_accept();
}

void GraphicsServer::do_accept()
{
	acceptor.async_accept(
		[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				std::shared_ptr<GraphicsSession> session = 
						std::make_shared<GraphicsSession>(std::move(socket), 
														  this->numConnections, 
							                              this->serverQueue,
														  this->queueMtx,
														  this->clientsConnected);
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
		session->do_write(serverFrame);
	}
} 