#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

int main()
{
    std::cout << "client start" << std::endl;
    boost::asio::io_service ios;
    boost::asio::ip::tcp::socket sock(ios);

    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 6677);
    sock.connect(ep);

    std::string str("hello world");
    sock.write_some(boost::asio::buffer(str));

    std::vector<char> v(100, 0);
    size_t n = sock.read_some(boost::asio::buffer(v));
    std::cout << &v[0] << std::endl;
}