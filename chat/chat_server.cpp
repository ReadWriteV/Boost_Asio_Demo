#include "chat_server.h"
#include "tcp_session.h"

#include <iostream>

#include <boost/bind/bind.hpp>
#include <boost/functional/factory.hpp>

chat_server::chat_server() : m_req_worker(m_recvq, boost::bind(&chat_server::process_msg, this, boost::placeholders::_1)), m_resp_worker(m_sendq, boost::bind(&chat_server::send_msg, this, boost::placeholders::_1))
{
}

chat_server::req_queue_type &chat_server::recv_queue()
{
    return m_recvq;
}

void chat_server::start()
{
    m_req_worker.start();
    m_resp_worker.start();
}

bool chat_server::process_msg(tcp_request_ptr &req)
{
    login_user(req);
    logout_user(req);
    deliver_msg(req);
    return true;
}

bool chat_server::send_msg(tcp_response_ptr &resp)
{
    resp->set_msg_crc();
    resp->get_session()->write(resp);
    return true;
}

void chat_server::login_user(tcp_request_ptr &req)
{
    auto hp = req->get_head();
    if (hp->type != state::CHAT_JOIN)
    {
        return;
    }
    std::string name(req->msg_data().data(), hp->size);
    BOOST_ASSERT(!m_sessions.find(name));
    std::cout << "login " << name << std::endl;

    m_sessions.insert(name, req->get_session());
}

void chat_server::logout_user(tcp_request_ptr &req)
{
    auto hp = req->get_head();
    if (hp->type != state::CHAT_LEAVE)
    {
        return;
    }
    std::string name(req->msg_data().data(), hp->size);
    BOOST_ASSERT(m_sessions.find(name));
    std::cout << "logout " << name << std::endl;

    m_sessions.erase(name);
}

void chat_server::deliver_msg(tcp_request_ptr &req)
{
    auto hp = req->get_head();
    if (hp->type != state::CHAT_MSG)
    {
        return;
    }
    std::cout << "deliver msg" << std::endl;
    m_sessions.for_each(boost::bind(&chat_server::map_call_back, this, boost::placeholders::_1, req));
}

void chat_server::map_call_back(map_type::reference x, tcp_request_ptr &req)
{
    auto sp = x.second;
    if (!sp->socket().is_open())
    {
        return;
    }
    auto resp = boost::factory<tcp_response_ptr>()(sp);
    resp->get_head()->size = req->get_head()->size;
    std::copy(req->msg_data().begin(), req->msg_data().begin() + req->get_head()->size, resp->msg_data().begin());

    m_sendq.push(resp);
}