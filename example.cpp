// g++ -I /Users/henrikjernbeck/Desktop/asio-1.30.2/include example.cpp -o example
// får väl se till att använda cmake när allt är klart sen.
// men orkar inte nu. 
 

#include <iostream>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <cmath>
#include <numeric>

#include <asio.hpp>

// Hämta data. borde vara något som ständigt bara lyssnar och slänger in varje rad
// kanske man bara gör en Struct Object.

struct Object
{
    Object() : ID(0), X(0), Y(0), type(0){color[0]=color[1]=color[2]=0;}
    Object(int64_t id, int32_t x, int32_t y, uint8_t type_param) : 
          ID(id), X(x), Y(y), type(type_param)
    {
        color[0]=color[1]=color[2]=0;
        if (type ==1 || type ==2)
        {
            category = 1;
        }
        else 
        {
            category =2;
        }
    }
    int64_t ID;
    int32_t X;
    int32_t Y;
    uint8_t type;
    uint8_t category;
    uint8_t color[3];

    void set_category()
    {
        if (type ==1 || type ==2)
        {
            category = 1;
        }
        else 
        {
            category =2;
        }
    }

    void set_color()
    {   
        set_category();
        int32_t x_ref{150};
        int32_t y_ref{150};

        int32_t dist = std::sqrt((X-x_ref)*(X-x_ref) + (Y - y_ref)*(Y - y_ref));

        // uint8_t RED[3] = {0x5B, 0x31, 0x6D};
        // uint8_t YELLOW[3] = {0x5B, 0x33, 0x6D};
        // uint8_t BLUE[3] = {0x5B, 0x34, 0x6D};
        if(category == 2)
        {
            if (dist < 100) // red type3
            {
                color[0]=0x5b;
                color[1]=0x31;
                color[2]=0x6d; 
            }
            else // yellow type3 
            {
                color[0]=0x5b;
                color[1]=0x33;
                color[2]=0x6D;   
            }
        }
        else if (category == 1)
        {
            if (type ==1)
            {
                if (dist < 50) // red type1
                {
                    color[0]=0x5b;
                    color[1]=0x31;
                    color[2]=0x6d;
                }
                else if (dist < 75) //yellow type1
                {
                    color[0]=0x5b;
                    color[1]=0x33;
                    color[2]=0x6D;
                }
                else // blue type 1
                {
                    color[0]=0x5b;
                    color[1]=0x34;
                    color[2]=0x6D;  
                }
            }
            else 
            {
                if (dist < 50) // yellow type 2
                {
                    color[0]=0x5b;
                    color[1]=0x33;
                    color[2]=0x6D;
                }
                else
                {
                    color[0]=0x5b;
                    color[1]=0x34;
                    color[2]=0x6D;  
                }
            }
            
        }
    }
};


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

    int stop = 0;
    std::vector<double> speed_check;
    for (std::string str; std::getline(input, str);)
    {   
        if (stop++ == 100)
        {
            break;
        }
        auto start_time = std::chrono::high_resolution_clock::now();
        Object temp;

        
        sscanf(str.c_str(), "ID=%lld;X=%d;Y=%d;TYPE=%hhu", &temp.ID, &temp.X, &temp.Y, &temp.type);

        temp.set_color();
        tracked_objects.insert_or_assign(temp.ID,std::move(temp));

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end_time - start_time;
        speed_check.push_back(duration.count());
        std::cout << "Time to create and insert in map: " << duration.count() << " seconds\n";



        std::cout << "color[1]: " <<std::hex << static_cast<int>(temp.color[1]) << " Category " << static_cast<int>(temp.category) << '\n';


    }
    // Lambda to calculate mean
    double mean = [&speed_check]() -> double {
        if (speed_check.empty()) {
            return 0.0;  // or handle the empty case as needed
        }
        return std::accumulate(speed_check.begin(), speed_check.end(), 0.0) / speed_check.size();
    }();

    std::cout << "Mean: " << mean << std::endl;



// här kan man säkert bara sätta en timer och köra på. 
// eller köra en thread. 
// jag antar att man kör 2 thread bara. server interaction och client interaction. 
// men då behöver man thread safe access till tracked obejcts. 
// sen en tråd som hanterar att skriva till Client. 
// färgen borde kanske göras när man skriver ut. man lägger på det. 


}