#pragma once

#include "tcp_handler.h"
#include "tcp_session.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/trivial.hpp>

#define LOG_TRACE(x) BOOST_LOG_TRIVIAL(trace) << x

class daytime_handler : public tcp_handler
{
public:
    daytime_handler()
    {
        handle_open = &daytime_handler::daytime_handle_open;
        handle_write = &daytime_handler::daytime_handle_write;
    }

private:
    static void daytime_handle_open(tcp_session_type session)
    {
        LOG_TRACE("daytime start from: ");
        LOG_TRACE(session->socket().remote_endpoint().address());

        auto ptime = boost::posix_time::microsec_clock::local_time();
        std::string str = boost::posix_time::to_simple_string(ptime);

        session->write(str.c_str(), str.size());
    }

    static void daytime_handle_write(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("write complete: " << n);
        session->close();
    }
};