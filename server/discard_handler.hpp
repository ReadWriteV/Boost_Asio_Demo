#pragma once

#include "tcp_handler.h"
#include "tcp_session.h"

#include <boost/log/trivial.hpp>

#define LOG_TRACE(x) BOOST_LOG_TRIVIAL(trace) << x

class discard_handler : public tcp_handler
{
public:
    discard_handler()
    {
        handle_open = &discard_handler::discard_handle_open;
        handle_read = &discard_handler::discard_handle_read;
    }

private:
    static void discard_handle_open(tcp_session_type session)
    {
        LOG_TRACE("discard start from: ");
        LOG_TRACE(session->socket().remote_endpoint().address());
    }

    static void discard_handle_read(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("read size: " << n);
        LOG_TRACE(std::string(session->read_buf().peek(), n));
    }
};