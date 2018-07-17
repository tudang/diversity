//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class udp_server
{
    public:
    udp_server(boost::asio::io_service& io_service)
        : socket_(io_service, udp::endpoint(udp::v4(), 12345)),
        tp_timer(io_service, boost::posix_time::seconds(1))

    {
        tp_timer.async_wait(boost::bind(&udp_server::measure_throughput, this));
        start_receive();
    }

    private:
    void start_receive()
    {
        socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_server::handle_receive, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error,
                        std::size_t /*bytes_transferred*/)
    {
        if (!error || error == boost::asio::error::message_size)
        {
            mps++;
            socket_.send_to(boost::asio::buffer(recv_buffer_), remote_endpoint_);
            start_receive();
        }
    }

    void measure_throughput()
    {
        std::cout << mps << std::endl;
        mps = 0;
        tp_timer.expires_at(tp_timer.expires_at() + boost::posix_time::seconds(1));
        tp_timer.async_wait(boost::bind(&udp_server::measure_throughput, this));
    }

    uint mps;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 128> recv_buffer_;
    boost::asio::deadline_timer tp_timer;

};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        udp_server server(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
