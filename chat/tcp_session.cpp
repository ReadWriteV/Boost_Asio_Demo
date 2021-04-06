#include "tcp_session.h"

#include <boost/bind/bind.hpp>

tcp_session::object_pool_type tcp_session::m_msg_pool;

tcp_session::tcp_session(ios_type &ios, queue_type &q) : m_socket(ios), m_strand(ios), m_queue(q)
{
}

tcp_session::socket_type &tcp_session::socket()
{
    return m_socket;
}

tcp_session::ios_type &tcp_session::io_service()
{
    return (boost::asio::io_context &)(m_socket).get_executor().context();
}

void tcp_session::start()
{
    tcp_request_ptr req = create_request();
    read(req);
}

void tcp_session::close()
{
    boost::system::error_code ignored_ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    m_socket.close(ignored_ec);
}

void tcp_session::write(tcp_response_ptr resp)
{
    boost::asio::async_write(m_socket, boost::asio::buffer(resp->head_data().data(), resp->head_data().size()), m_strand.wrap(boost::bind(&tcp_session::handle_write_head, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)));
}

tcp_request_ptr tcp_session::create_request()
{
    return m_msg_pool.construct(shared_from_this(), boost::bind(&object_pool_type::destroy, boost::ref(m_msg_pool), boost::placeholders::_1));
}

void tcp_session::read(tcp_request_ptr req)
{
    boost::asio::async_read(m_socket, boost::asio::buffer(req->head_data().data(), req->head_data().size()), m_strand.wrap(boost::bind(&tcp_session::handle_read_head, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)));
}

void tcp_session::handle_read_head(const boost::system::error_code &error, size_t bytes_transferred, tcp_request_ptr req)
{
    if (error || !req->check_head())
    {
        close();
        return;
    }

    boost::asio::async_read(m_socket, boost::asio::buffer(req->msg_data().data(), req->get_head()->size), m_strand.wrap(boost::bind(&tcp_session::handle_read_msg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, req)));
}

void tcp_session::handle_read_msg(const boost::system::error_code &error, size_t bytes_transferred, tcp_request_ptr req)
{
    if (error || !req->check_msg_crc())
    {
        close();
        return;
    }

    m_queue.push(req);

    start();
}

void tcp_session::handle_write_head(const boost::system::error_code &error, size_t bytes_transferred, tcp_response_ptr resp)
{
    if (error || bytes_transferred != resp->head_data().size())
    {
        close();
        return;
    }
    boost::asio::async_write(m_socket, boost::asio::buffer(resp->msg_data().data(), resp->get_head()->size), m_strand.wrap(boost::bind(&tcp_session::handle_write_msg, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp)));
}
void tcp_session::handle_write_msg(const boost::system::error_code &error, size_t bytes_transferred, tcp_response_ptr resp)
{
    if (error || bytes_transferred != resp->get_head()->size)
    {
        close();
        return;
    }
    resp->destory();
}
