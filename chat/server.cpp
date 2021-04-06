#include "tcp_server.hpp"
#include "chat_server.h"

int main()
{
    chat_server cs;
    cs.start();

    tcp_server srv(6688, cs.recv_queue(), 1);
    srv.run();
}