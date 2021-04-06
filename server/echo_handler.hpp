#pragma once

#include "tcp_handler.h"
#include "tcp_session.h"

#include <boost/log/trivial.hpp>

#define LOG_TRACE(x) BOOST_LOG_TRIVIAL(trace) << x

class echo_handler : public tcp_handler
{
public:
    echo_handler()
    {
        handle_open = &echo_handler::echo_handle_open;
        handle_close = &echo_handler::echo_handle_close;
        handle_read = &echo_handler::echo_handle_read;
        handle_write = &echo_handler::echo_handle_write;
    }

private:
    static void echo_handle_open(tcp_session_type session)
    {
        LOG_TRACE("session start from: ");
        LOG_TRACE(session->socket().remote_endpoint().address());
    }

    static void echo_handle_close(tcp_session_type session)
    {
        LOG_TRACE("session close");
    }

    static void echo_handle_read(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("read size: " << n);
        LOG_TRACE(std::string(session->read_buf().peek(), n));

        session->write(session->read_buf().peek(), n);
    }

    static void echo_handle_write(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("write complete: " << n);
    }
};