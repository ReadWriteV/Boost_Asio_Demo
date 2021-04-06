#pragma once

#include "intdef.h"
#include "tcp_session.h"
#include "io_service_pool.hpp"

#include <boost/bind/bind.hpp>

template <typename Handler>
class tcp_server
{
public:
    typedef io_service_pool::ios_type ios_type;
    tcp_server(ushort_t port, int n = 1) : m_ios_pool(*boost::factory<io_service_pool *>()(n)), m_acceptor(m_ios_pool.get(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        start_accept();
    }
    tcp_server(io_service_pool &ios, ushort_t port) : m_ios_pool(ios), m_acceptor(m_ios_pool.get(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
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
        tcp_session_ptr session = boost::factory<tcp_session_ptr>()(m_ios_pool.get());
        m_acceptor.async_accept(session->socket(), boost::bind(&tcp_server::handle_accept, this, boost::asio::placeholders::error, session));
    }
    void handle_accept(const boost::system::error_code &error, tcp_session_ptr session)
    {
        start_accept();
        if (error)
        {
            session->close();
            return;
        }
        // session->start(m_handler);
        session->io_service().dispatch(boost::bind(&tcp_session::start, session, m_handler));
    }

    io_service_pool &m_ios_pool;
    typedef boost::asio::ip::tcp::acceptor acceptor_type;
    acceptor_type m_acceptor;
    typedef Handler handler_type;
    handler_type m_handler;
};
