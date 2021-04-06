#include <iostream>

#include "tcp_server.hpp"
#include "http_handler.hpp"

int main()
{
    std::cout << "server start" << std::endl;
    tcp_server<http_handler> server(6677, 2);
    server.run();
}