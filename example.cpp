#include <iostream>
#include <string>
#include <asio.hpp>

int main() 
{
    asio::ip::tcp::iostream input("localhost", "5463"); 
    for (std::string str; std::getline(input, str);)
        std::cout << str << '\n';
}