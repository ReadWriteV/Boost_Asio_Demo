#pragma once

#include "tcp_handler.h"
#include "tcp_session.h"

#include <boost/log/trivial.hpp>

#define LOG_TRACE(x) BOOST_LOG_TRIVIAL(trace) << x

class time_handler : public tcp_handler
{
public:
    time_handler()
    {
        handle_open = &time_handler::time_handle_open;
        handle_write = &time_handler::time_handle_write;
    }

private:
    static void time_handle_open(tcp_session_type session)
    {
        LOG_TRACE("time start from: ");
        LOG_TRACE(session->socket().remote_endpoint().address());

        int32_t t = boost::asio::detail::socket_ops::host_to_network_long(static_cast<int>(time(0)));

        session->write(&t, sizeof(t));
    }

    static void time_handle_write(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("write complete: " << n);
        session->close();
    }
};