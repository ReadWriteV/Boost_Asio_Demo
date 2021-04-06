#pragma once

#include "tcp_handler.h"
#include "tcp_session.h"

#include <boost/circular_buffer.hpp>
#include <boost/next_prior.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/log/trivial.hpp>

#define LOG_TRACE(x) BOOST_LOG_TRIVIAL(trace) << x

class chargen_handler : public tcp_handler
{
public:
    chargen_handler()
    {
        handle_open = &chargen_handler::chargen_handle_open;
        handle_write = &chargen_handler::chargen_handle_write;
    }

private:
    static void chargen_handle_open(tcp_session_type session)
    {
        LOG_TRACE("chargen start from: ");
        LOG_TRACE(session->socket().remote_endpoint().address());
        write(session);
    }

    static void chargen_handle_write(tcp_session_type session, std::size_t n)
    {
        LOG_TRACE("write complete: " << n);
        session->close();
        write(session);
    }

    typedef boost::circular_buffer<char> cb_type;
    static void write(tcp_session_type session)
    {
        static cb_type msg(0x7f - 0x20);
        if (msg.empty())
        {
            init(msg);
        }
        std::vector<char> tmp(msg.begin(), msg.begin() + 72);

        session->write_buf().append(&tmp[0], tmp.size());
        session->write_buf().append("\n", 1);
        session->write();

        msg.rotate(boost::next(msg.begin()));
    }

    static void init(cb_type &msg)
    {
        boost::copy(boost::counting_range(0x20, 0x7f), std::back_inserter(msg));
    }
};