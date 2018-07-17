//
// blocking_udp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <boost/asio.hpp>
#include <time.h>

using boost::asio::ip::udp;

enum { max_length = 1024 };

void send_timestamp(udp::socket& s, udp::resolver::iterator& iterator)
{
    auto ts = std::chrono::high_resolution_clock::now();
    int msg_size = sizeof ts;
    s.send_to(boost::asio::buffer((char*)&ts, msg_size), *iterator);
}

void on_read(udp::socket& s)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto msg_size = sizeof start;
    udp::endpoint sender_endpoint;
    size_t reply_length = s.receive_from(
        boost::asio::buffer((char*)&start, msg_size), sender_endpoint);

    auto end = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    std::cout << latency << std::endl;

    s.send_to(boost::asio::buffer((char*)&end, sizeof end), sender_endpoint);
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        udp::socket s(io_service, udp::endpoint(udp::v4(), 0));

        udp::resolver resolver(io_service);
        udp::resolver::query query(udp::v4(), argv[1], argv[2]);
        udp::resolver::iterator iterator = resolver.resolve(query);

        send_timestamp(s, iterator);
        while (1) {
            on_read(s);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
