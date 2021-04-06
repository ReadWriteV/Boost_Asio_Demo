#pragma once

#include "tcp_handler.h"
#include "tcp_session.h"

#include <boost/log/trivial.hpp>

#define LOG_TRACE(x) BOOST_LOG_TRIVIAL(trace) << x

class http_handler : public tcp_handler
{
public:
    http_handler()
    {
        handle_read = &http_handler::http_handle_read;
        handle_write = &http_handler::http_handle_write;
    }

private:
    static void http_handle_read(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("http start from: ");
        LOG_TRACE(session->socket().remote_endpoint().address());

        std::string content(1024, 'a');
        std::string str =
            "HTTP/1.0 200 OK\r\n"
            "Content-Length: 1024\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n";
        str += content;

        session->write(str.c_str(), str.size());
    }

    static void http_handle_write(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("write complete: " << n);
        session->close();
    }
};