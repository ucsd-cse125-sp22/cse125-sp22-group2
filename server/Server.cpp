#include "Server.hpp"

//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <deque>
#include <boost/asio.hpp>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

GraphicsSession::GraphicsSession(boost::asio::ip::tcp::socket socket, int myid, std::deque<cse125framing::ClientFrame>& serverQueue, unsigned int& clientsConnected)
	: socket(std::move(socket)), id(myid), serverQueue(serverQueue), clientsConnected(clientsConnected)
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

	boost::array<char, cse125framing::CLIENT_FRAME_BUFFER_SIZE> clientBuffer;

	socket.async_read_some(boost::asio::buffer(clientBuffer), 
		[&clientBuffer, this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				cse125framing::ClientFrame clientFrame;

				// process data
				cse125framing::deserialize(&clientFrame, clientBuffer);

				std::cerr << "Frame from client: " << std::endl;
				std::cerr << &clientFrame << std::endl;

				// Check if ID needs to be sent back
				if (clientFrame.id == cse125constants::DEFAULT_CLIENT_ID)
				{
					// lock
					std::cerr << "Giving client id: " << this->id << std::endl;
					cse125framing::ServerFrame frame;
					frame.id = this->id;
					do_write(&frame);
					// free lock
				}
				else
				{
					// write to packet buffer (queue)
					this->serverQueue.push_back(clientFrame);
					std::cerr << "Server queue size: " << this->serverQueue.size() << std::endl;
				}

				// read more packets
				do_read();
			}
		});
}

void GraphicsSession::do_write(cse125framing::ServerFrame* serverFrame)
{
	auto self(shared_from_this());
	boost::array<char, cse125framing::SERVER_FRAME_BUFFER_SIZE> serverBuffer;

	cse125framing::serialize(serverFrame, serverBuffer); 

	boost::asio::async_write(socket, boost::asio::buffer(serverBuffer),
		[&serverFrame, this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{				
				std::cerr << "Frame written to client" << std::endl;

				// TODO: Accessing serverFrame in this if statement throws an exception

				if (this->clientsConnected < cse125constants::NUM_PLAYERS) {
					this->clientsConnected++;
				}
				std::cerr << "Clients connected: " << this->clientsConnected << std::endl;
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
	// boost::asio::ip::tcp::endpoint endpoint = acceptor.local_endpoint();
	// std::cout << endpoint.address() << std::endl;
	// std::cout << endpoint.port() << std::endl;

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