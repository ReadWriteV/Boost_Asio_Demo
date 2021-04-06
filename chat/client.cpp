#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "chat_client.hpp"

int main()
{
    std::cout << "chat client" << std::endl;

    boost::asio::io_service ios;

    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 6688);

    chat_client client(ios, ep);

    boost::thread t(boost::bind(&boost::asio::io_service::run, &ios));

    std::string name;
    std::cout << "please input name: ";
    std::getline(std::cin, name);
    client.login(name);

    std::string str;

    while (std::getline(std::cin, str))
    {
        if (str == "quit")
        {
            break;
        }
        str = name + " say: " + str;
        client.send(str);
    }

    client.logout(name);
}