// g++ -I /Users/henrikjernbeck/Desktop/asio-1.30.2/include testserver.cpp -o testserver

#include <iostream>
#include <asio.hpp>
#include <thread>

int main() {
    asio::io_context io_context;
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 8080);
    asio::ip::tcp::acceptor acceptor(io_context, endpoint);

    for (;;)
    {
        asio::ip::tcp::iostream stream;
        std::error_code ec;
        acceptor.accept(stream.socket(), ec);
        if (!ec)
        {
            while (true) {
                if (!(stream << "Hello!" << std::endl)) {
                    std::cerr << "Connection lost!" << std::endl;
                    break;  // Exit the loop if the connection is lost
                }
                stream.flush();  // Make sure data is sent immediately
                
                // Check for any stream errors
                if (stream.fail()) {
                    std::cerr << "Connection error!" << std::endl;
                    break;  // Exit the loop on connection error
                }

                std::this_thread::sleep_for(std::chrono::seconds(1));  // Delay between messages
            }
        } else {
            std::cerr << "Failed to accept connection!" << std::endl;
        }
    }
}
