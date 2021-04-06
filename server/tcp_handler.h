#pragma once

#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

class tcp_session;
typedef const boost::shared_ptr<tcp_session> &tcp_session_type;

struct tcp_handler
{
    boost::function<void(tcp_session_type)> handle_open;
    boost::function<void(tcp_session_type)> handle_close;

    boost::function<void(tcp_session_type, std::size_t)> handle_read;
    boost::function<void(tcp_session_type, std::size_t)> handle_write;
};
