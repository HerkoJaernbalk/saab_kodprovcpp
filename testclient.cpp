// g++ -I /Users/henrikjernbeck/Desktop/asio-1.30.2/include testclient.cpp -o testclient

#include <iostream>
#include <string>
#include <asio.hpp>
int main() {
    asio::ip::tcp::iostream input("localhost", "8080"); 
    for (std::string str; std::getline(input, str);)
    {
        std::cout << str << '\n';
    }
}