#pragma once

#include "tcp_message.hpp"
#include "job_queue.hpp"
#include "worker.hpp"
#include "safe_map.hpp"

enum state
{
    CHAT_JOIN,
    CHAT_MSG,
    CHAT_LEAVE
};

class chat_server
{
public:
    typedef job_queue<tcp_request_ptr> req_queue_type;
    typedef worker<req_queue_type> req_worker_type;

    typedef job_queue<tcp_response_ptr> resp_queue_type;
    typedef worker<resp_queue_type> resp_worker_type;

    typedef safe_map<std::string, tcp_session_ptr> map_type;

    chat_server();
    req_queue_type &recv_queue();
    void start();

private:
    bool process_msg(tcp_request_ptr &req);
    bool send_msg(tcp_response_ptr &resp);

    void login_user(tcp_request_ptr &req);
    void logout_user(tcp_request_ptr &req);
    void deliver_msg(tcp_request_ptr &req);
    void map_call_back(map_type::reference x, tcp_request_ptr &req);

    req_queue_type m_recvq;
    resp_queue_type m_sendq;

    req_worker_type m_req_worker;
    resp_worker_type m_resp_worker;

    map_type m_sessions;
};