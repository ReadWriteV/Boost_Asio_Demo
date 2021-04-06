#pragma once

#include "tcp_message.hpp"
#include "job_queue.hpp"

#include <boost/smart_ptr.hpp>
#include <boost/pool/object_pool.hpp>

#include <boost/asio.hpp>

class tcp_session : public boost::enable_shared_from_this<tcp_session>
{
public:
    typedef boost::asio::ip::tcp::socket socket_type;
    typedef boost::asio::io_service ios_type;

    typedef ios_type::strand strand_type;
    typedef job_queue<tcp_request_ptr> queue_type;
    typedef boost::object_pool<tcp_message> object_pool_type;

    tcp_session(ios_type &ios, queue_type &q);
    socket_type &socket();
    ios_type &io_service();

    void start();
    void close();

    void write(tcp_response_ptr resp);

private:
    tcp_request_ptr create_request();
    void read(tcp_request_ptr req);

    void handle_read_head(const boost::system::error_code &error, size_t bytes_transferred, tcp_request_ptr req);
    void handle_read_msg(const boost::system::error_code &error, size_t bytes_transferred, tcp_request_ptr req);
    void handle_write_head(const boost::system::error_code &error, size_t bytes_transferred, tcp_response_ptr resp);
    void handle_write_msg(const boost::system::error_code &error, size_t bytes_transferred, tcp_response_ptr resp);

    socket_type m_socket;
    strand_type m_strand;
    queue_type &m_queue;
    static object_pool_type m_msg_pool;
};

typedef boost::shared_ptr<tcp_session> tcp_session_ptr;