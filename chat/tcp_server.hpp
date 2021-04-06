#pragma once

#include "tcp_session.h"
#include "io_service_pool.hpp"

#include <boost/bind/bind.hpp>
#include <boost/functional/factory.hpp>

class tcp_server
{
public:
    typedef tcp_session::ios_type ios_type;
    typedef boost::asio::ip::tcp::acceptor acceptor_type;
    typedef boost::asio::ip::tcp tcp_type;
    typedef tcp_session::queue_type queue_type;

    tcp_server(ushort_t port, queue_type &q, int n = 4) : m_ios_pool(*boost::factory<io_service_pool *>()(n)), m_queue(q), m_acceptor(m_ios_pool.get(), tcp_type::endpoint(tcp_type::v4(), port))
    {
        start_accept();
    }
    tcp_server(io_service_pool &ios, ushort_t port, queue_type &q) : m_ios_pool(ios), m_queue(q), m_acceptor(m_ios_pool.get(), tcp_type::endpoint(tcp_type::v4(), port))
    {
        start_accept();
    }
    void start()
    {
        m_ios_pool.start();
    }
    void run()
    {
        m_ios_pool.run();
    }

private:
    void start_accept()
    {
        tcp_session_ptr session = boost::factory<tcp_session_ptr>()(m_ios_pool.get(), m_queue);
        m_acceptor.async_accept(session->socket(), boost::bind(&tcp_server::accept_handler, this, boost::asio::placeholders::error, session));
    }
    void accept_handler(const boost::system::error_code &error, tcp_session_ptr session)
    {
        start_accept();
        if (error)
        {
            session->close();
            return;
        }
        // session->start();
        session->io_service().dispatch(boost::bind(&tcp_session::start, session));
    }

    io_service_pool &m_ios_pool;
    queue_type &m_queue;
    acceptor_type m_acceptor;
};
