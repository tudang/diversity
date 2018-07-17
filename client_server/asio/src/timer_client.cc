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
#include <fstream>
#include <chrono>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::udp;

using namespace std;
using namespace std::chrono;

enum { max_length = 1024 };

class timer_client
{
public:
    timer_client(boost::asio::io_service& io_service, char *remote, char* remote_port)
        : socket_(io_service, udp::endpoint(udp::v4(), 0))
    {
        udp::resolver resolver(io_service);
        udp::resolver::query query(udp::v4(), remote, remote_port);
        iterator = resolver.resolve(query);
        cout << "local port " << socket_.local_endpoint().port() << "\n";
        std::string fname = "latency-" + std::to_string(socket_.local_endpoint().port()) + ".txt";
        std::cout << "latency file name: " << fname << '\n';
        lat_ofs = ofstream(fname);
        count = 0;
        send_timestamp();
        start_receive();
    }

private:
    void send_timestamp()
    {
        start = high_resolution_clock::now();
        uint64_t start_ns = duration_cast<nanoseconds>(start.time_since_epoch()).count();
        shared_ptr<std::string> s = make_shared<std::string>(std::to_string(start_ns));
        socket_.send_to(boost::asio::buffer(*s), *iterator);
    }

    void start_receive()
    {
        socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_, max_length), remote_endpoint_,
        boost::bind(&timer_client::handle_receive, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error,
                        std::size_t /*bytes_transferred*/)
    {
        if (!error || error == boost::asio::error::message_size)
        {
            count++;
            auto end = high_resolution_clock::now();
            uint64_t end_ns = duration_cast<nanoseconds>(end.time_since_epoch()).count();
            uint64_t start_ns;
            std::istringstream iss(recv_buffer_.data());
            if (iss >> start_ns) {
                uint64_t latency = end_ns - start_ns;
                lat_ofs << latency << std::endl;
            }
            shared_ptr<std::string> s = make_shared<std::string>(std::to_string(end_ns));

            socket_.send_to(boost::asio::buffer((*s)), remote_endpoint_);
            if (count > 100000) {
                socket_.get_io_service().stop();
            }
            start_receive();
        }
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    udp::resolver::iterator iterator;
    boost::array<char, max_length> recv_buffer_;
    ofstream lat_ofs;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    int count;
};


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

        timer_client timer_client(io_service, argv[1], argv[2]);
        io_service.run();

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
