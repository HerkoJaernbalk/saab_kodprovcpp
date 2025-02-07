// g++ -I /Users/henrikjernbeck/Desktop/asio-1.30.2/include example.cpp -o example
// får väl se till att använda cmake när allt är klart sen.
// men orkar inte nu. 
 

#include <iostream>
#include <string>
#include <cstdint>
#include <unordered_map>

#include <asio.hpp>

// Hämta data. borde vara något som ständigt bara lyssnar och slänger in varje rad
// kanske man bara gör en Struct Object.

struct Object
{
    Object() : ID(0), X(0), Y(0), type(0) {}
    Object(int64_t id, int32_t x, int32_t y, uint8_t type_param) : 
          ID(id), X(x), Y(y), type(type_param) {}
    int64_t ID;
    int32_t X;
    int32_t Y;
    uint8_t type;
};


Object parse_line(const std::string& line)
{
    std::unordered_map<std::string, std::string> key_value_map;

    // Split the string by semicolons to get key-value pairs
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ';'))
    {
        size_t pos = token.find('=');
        if (pos != std::string::npos)
        {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            key_value_map[key] = value;
        }
    }

    // Convert the extracted values into the right types
    int64_t id = std::stoll(key_value_map["ID"]);
    int32_t x = std::stoi(key_value_map["X"]);
    int32_t y = std::stoi(key_value_map["Y"]);
    uint8_t type = static_cast<uint8_t>(std::stoi(key_value_map["TYPE"]));

    // Create and return the Object

    std::cout << "object created" << std::endl;


    return Object(id, x, y, type);
}

void handle_client()
{
    // får fundera på det här senare, kan börja med att printa ut skiten i terminalen.
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
            stream << "Test!";
        }
    }
}


int main()
{
    // en map med object. borde funka ok! 
    std::unordered_map<int64_t, Object> tracked_objects;
    asio::ip::tcp::iostream input("localhost", "5463");

    // mycket nonsens här nu bara för att testa. 

    for (std::string str; std::getline(input, str);)
    {
        Object temp = parse_line(str);
        tracked_objects[temp.ID] = temp;
        std::cout << str << '\n';
    }

    // sen en tråd som hanterar att skriva till Client. 
    // färgen borde kanske göras när man skriver ut. man lägger på det. 


}