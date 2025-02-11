// g++ -I /Users/henrikjernbeck/Desktop/asio-1.30.2/include testclient.cpp -o testclient

#include <iostream>
#include <string>
#include <asio.hpp>
#include <thread>


struct Object
{
    Object() : ID(0), X(0), Y(0), type(0){color[0]=color[1]=color[2]=0;}
    Object(int64_t id, int32_t x, int32_t y, uint8_t type_param) : 
          ID(id), X(x), Y(y), type(type_param){color[0]=color[1]=color[2]=0;}
    int64_t ID;
    int32_t X;
    int32_t Y;
    uint8_t type;
    uint8_t color[3];
};

int main() {
    asio::ip::tcp::iostream input("localhost", "5000");
    for (std::string str; std::getline(input, str);)
    {
        // Object temp;
        // sscanf(str.c_str(), "ID=%lld;X=%d;Y=%d;TYPE=%hhu", &temp.ID, &temp.X, &temp.Y, &temp.type);
        // std::cout << temp.type << '\n';
        //std::this_thread::sleep_for(std::chrono::seconds(1));  // Delay between messages
        std::cout << str <<  std::endl;

    }
}


// nu vet jag att meddelanden inte försvinner
// om det är helt olika hastighet på server och client. 
// det verkar bara vara att det hamnar i en kö. 
// Det här var ett bra experiemnt för att se att jag inte missar information.
// känns dock som att det kanske kan bli fullt i kön på något sätt. 
// Det är alltså bara en fråga om hur snabb man kan göra middlewares hantering av data. 