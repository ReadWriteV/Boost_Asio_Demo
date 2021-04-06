#include "tcp_session.h"

#include <boost/bind/bind.hpp>

tcp_session::tcp_session(ios_type &ios) : m_socket(ios)
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

tcp_session::buffer_type &tcp_session::read_buf()
{
    return m_read_buf;
}

tcp_session::buffer_type &tcp_session::write_buf()
{
    return m_write_buf;
}

void tcp_session::start(tcp_handler handler)
{
    m_handler = handler;
    if (m_handler.handle_open)
    {
        m_handler.handle_open(shared_from_this());
    }
    read();
}

void tcp_session::close()
{
    boost::system::error_code ignored_ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    m_socket.close(ignored_ec);

    if (m_handler.handle_close)
    {
        m_handler.handle_close(shared_from_this());
    }
}

void tcp_session::read()
{
    m_socket.async_read_some(m_read_buf.prepare(), boost::bind(&tcp_session::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void tcp_session::handle_read(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (error)
    {
        close();
        return;
    }

    m_read_buf.retrieve(bytes_transferred);

    if (m_handler.handle_read)
    {
        m_handler.handle_read(shared_from_this(), bytes_transferred);
    }

    m_read_buf.consume(bytes_transferred);
    read();
}

void tcp_session::write(const void *data, std::size_t len)
{
    m_write_buf.append(data, len);

    write();
}

void tcp_session::write()
{
    m_socket.async_write_some(m_write_buf.data(), boost::bind(&tcp_session::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void tcp_session::handle_write(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (error)
    {
        close();
        return;
    }
    m_write_buf.consume(bytes_transferred);
    if (m_handler.handle_write)
    {
        m_handler.handle_write(shared_from_this(), bytes_transferred);
    }
}