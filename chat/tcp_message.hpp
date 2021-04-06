#pragma once

#include "tcp_msg_def.h"

#include <boost/checked_delete.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/crc.hpp>

class tcp_session;
typedef boost::shared_ptr<tcp_session> tcp_session_ptr;

class tcp_message : boost::noncopyable
{
public:
    typedef boost::function<void(tcp_message *)> destory_type;
    typedef char char_type;
    typedef boost::array<char_type, sizeof(msg_head)> head_data_type;
    typedef boost::array<char_type, MAX_BODY_SIZE> body_data_type;

    template <typename Func>
    tcp_message(const tcp_session_ptr &s, Func func) : m_session(s), m_destory(func)
    {
    }

    tcp_message(const tcp_session_ptr &s) : m_session(s)
    {
    }

    tcp_session_ptr get_session()
    {
        return m_session;
    }

    void destory()
    {
        if (m_destory)
        {
            m_destory(this);
        }
        else
        {
            boost::checked_delete(this);
        }
    }

    head_data_type &head_data()
    {
        return m_head;
    }

    body_data_type &msg_data()
    {
        return m_msg;
    }

    msg_head *get_head()
    {
        return reinterpret_cast<msg_head *>(m_head.data());
    }

    bool check_head()
    {
        return (get_head()->size < MAX_BODY_SIZE);
    }

    bool check_msg_crc()
    {
        boost::crc_32_type crc32;
        crc32.process_bytes(&m_msg[0], get_head()->size);
        return get_head()->chksum == crc32.checksum();
    }

    void set_msg_crc()
    {
        boost::crc_32_type crc32;
        crc32.process_bytes(&m_msg[0], get_head()->size);
        get_head()->chksum = crc32.checksum();
    }

private:
    tcp_session_ptr m_session;
    destory_type m_destory;

    head_data_type m_head;
    body_data_type m_msg;
};

typedef tcp_message tcp_request;
typedef tcp_message tcp_response;
typedef tcp_request *tcp_request_ptr;
typedef tcp_response *tcp_response_ptr;