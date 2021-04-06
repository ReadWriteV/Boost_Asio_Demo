#pragma once

#include "tcp_msg_def.h"

#include <iostream>
#include <deque>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/crc.hpp>

class chat_client
{
public:
    typedef boost::asio::ip::tcp::socket socket_type;
    typedef boost::asio::io_service ios_type;
    typedef boost::asio::ip::tcp tcp_type;

    chat_client(ios_type &ios, tcp_type::endpoint ep) : m_ios(ios), m_socket(ios)
    {
        start_connect(ep);
    }

    void send(const std::string &str)
    {
        m_send_msgs.push_back(str);
        write();
    }

    void login(const std::string &name)
    {
        msg_head head;
        head.type = 0;
        head.size = static_cast<uint32_t>(name.size());
        head.chksum = std::for_each(name.begin(), name.end(), boost::crc_32_type())();

        m_socket.write_some(boost::asio::buffer(&head, sizeof(head)));
        m_socket.write_some(boost::asio::buffer(name));
    }

    void logout(const std::string &name)
    {
        msg_head head;
        head.type = 2;
        head.size = static_cast<uint32_t>(name.size());
        head.chksum = std::for_each(name.begin(), name.end(), boost::crc_32_type())();

        m_socket.write_some(boost::asio::buffer(&head, sizeof(head)));
        m_socket.write_some(boost::asio::buffer(name));
    }

    void close()
    {
        m_socket.close();
    }

private:
    void start_connect(tcp_type::endpoint ep)
    {
        m_socket.async_connect(ep, boost::bind(&chat_client::handle_connect, this, boost::asio::placeholders::error));
    }

    void handle_connect(const boost::system::error_code &error)
    {
        read();
    }

    void read()
    {
        boost::asio::async_read(m_socket, boost::asio::buffer(&m_read_head, sizeof(m_read_head)), boost::bind(&chat_client::handle_read_head, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void handle_read_head(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error)
        {
            return;
        }
        boost::asio::async_read(m_socket, boost::asio::buffer(&m_read_data[0], m_read_head.size), boost::bind(&chat_client::handle_read_msg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void handle_read_msg(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error)
        {
            return;
        }
        std::cout.write(&m_read_data[0], m_read_head.size);
        std::cout << std::endl;

        read();
    }

    void write()
    {
        msg_head head;
        const std::string &str = m_send_msgs.front();
        head.type = 1;
        head.size = static_cast<uint32_t>(str.size());
        head.chksum = std::for_each(str.begin(), str.end(), boost::crc_32_type())();

        m_socket.write_some(boost::asio::buffer(&head, sizeof(head)));
        m_socket.write_some(boost::asio::buffer(str));

        m_send_msgs.pop_front();
    }

    ios_type &m_ios;
    socket_type m_socket;

    msg_head m_read_head;
    boost::array<char, 1024> m_read_data;
    std::deque<std::string> m_send_msgs;
};