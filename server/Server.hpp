#pragma once
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
#include <boost/asio.hpp>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

class GraphicsSession : public std::enable_shared_from_this<GraphicsSession>
{
public:
    GraphicsSession(boost::asio::ip::tcp::socket socket)
        : socket(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
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
   
                    // write to packet buffer (queue)

                    // read more packets
                    do_read();
                }
            });
    }

    void do_write(cse125framing::ServerFrame* serverFrame)
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
                    std::cerr << serverFrame << std::endl;
                }
                else
                {
                    std::cerr << "Write failed.. NOT rewriting" << std::endl;
                    // do_write(serverFrame);
                }
            });
    }

    boost::asio::ip::tcp::socket socket;

};

class GraphicsServer
{
public:
    GraphicsServer(boost::asio::io_context& io_context, short port)
        : acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        numConnections = 0;
        boost::asio::ip::tcp::endpoint endpoint = acceptor.local_endpoint();
        std::cout << endpoint.address() << std::endl;
        std::cout << endpoint.port() << std::endl;
        do_accept();
    }*/

private:
    void do_accept()
    {
        acceptor.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<GraphicsSession>(std::move(socket))->start();

                    this->numConnections++;
                }

                // only accept 4 connections
                if (this->numConnections < cse125constants::NUM_PLAYERS)
                {
                    do_accept();
                }
            });
    }

    boost::asio::ip::tcp::acceptor acceptor;
    unsigned int numConnections;
};
