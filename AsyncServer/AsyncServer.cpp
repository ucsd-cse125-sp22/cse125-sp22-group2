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
        auto self(shared_from_this());
        socket.async_read_some(boost::asio::buffer(data, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket, boost::asio::buffer(data, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
            });
    }

    boost::asio::ip::tcp::socket socket;
    enum { max_length = 1024 };
    char data[max_length];
};

class GraphicsServer
{
public:
    GraphicsServer(boost::asio::io_context& io_context, short port)
        : acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        boost::asio::ip::tcp::endpoint endpoint = acceptor.local_endpoint();
        std::cout << endpoint.address() << std::endl;
        std::cout << endpoint.port() << std::endl;
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<GraphicsSession>(std::move(socket))->start();
                }

                do_accept();
            });
    }

    boost::asio::ip::tcp::acceptor acceptor;
};

void launchServer(short port) {
    boost::asio::io_context io_context;

    GraphicsServer s(io_context, port);

    std::cout << "Before io_context.run()" << std::endl;
    io_context.run();
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }
        launchServer(8000);

 /*       boost::thread thr{ launchServer, std::atoi(argv[1])};
        thr.join();*/
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    std::cout << "Before end of main()" << std::endl;



    return 0;
}