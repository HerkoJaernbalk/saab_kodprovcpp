// g++ -I /Users/henrikjernbeck/Desktop/asio-1.30.2/include example.cpp -o example
// får väl se till att använda cmake när allt är klart sen.
 
#include <iostream>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <cmath>
#include <numeric>
#include <bitset>

#include <asio.hpp>

// Hämta data. borde vara något som ständigt bara lyssnar och slänger in varje rad
// kanske man bara gör en Struct Object.


// fin lösning xD. 
bool CONNECTION_FLAG = true;

struct Object
{
    Object() : ID(0), X(0), Y(0), type(0){color[0]=color[1]=color[2]=0;}
    // behövs inte.
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
    
    //struct member function that serialize object. 
    void serialize(std::ostream &out) const {
        out.write(reinterpret_cast<const char*>(&ID), sizeof(ID));
        out.write(reinterpret_cast<const char*>(&X), sizeof(X));
        out.write(reinterpret_cast<const char*>(&Y), sizeof(Y));
        out.write(reinterpret_cast<const char*>(&type), sizeof(type));
        out.write(reinterpret_cast<const char*>(color), sizeof(color));
    }
};



void read_from_saab(const std::string &ip, const std::string &port, std::unordered_map<int64_t, Object> &tracked_objects, std::mutex &maplock)
{
    int retries = 0;
    while (retries < 10) // vi testar att få tillbaka koppling mot server. 
    {
        try
        {
            asio::ip::tcp::iostream input(ip, port);
            if (!input)
            {
                throw std::runtime_error("Connection error no input stream");
            }
            else
            {
                retries = 0; //snyggt! :D
            }

            for (std::string str; std::getline(input, str);)
            {
                Object temp; //kanske värt att lägga det här efter testet. 
                             //men det är så "enkel" struct och constructor så det kanske inte gör så mycket.
                             // man kan ha tillfälliga variabler som man sedan bygger Object från och struntar i default constructorn.
                             // det skulle dock göras efter count == 4.
                int count = sscanf(str.c_str(), "ID=%lld;X=%d;Y=%d;TYPE=%hhu", &temp.ID, &temp.X, &temp.Y, &temp.type);
                if (count != 4)
                {
                    std::clog << "parse error\n";
                }
                else
                {
                    temp.set_color();
                    {
                        std::scoped_lock lock(maplock);
                        tracked_objects.insert_or_assign(temp.ID, std::move(temp)); // flyttar istället för kopiera!
                                                                                    // jag är ett geni! och sparar massor med tid. xD 
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            std::clog << "Connection error: " << e.what() << '\n';
        }

        std::clog << "Connection lost. Retrying...\n";
        retries++;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // vänta lite innan man testar igen 
    }
    std::clog << "Max retries reached. Stopping.\n";
    // här skulle man sätta connection_flag.
    CONNECTION_FLAG = false;
}

//den här var när jag missuppfattade vad jag skulle göra. 
//jag tänkte att middleware skulle kunna skicka på samma sätt som den tar emot. 
//Den här gör inget speciellt, hanterar ingen data
/*

void send_to_client(const int &port, std::unordered_map<int64_t,Object> &tracked_objects, std::mutex &maplock)
{
    asio::io_context io_context;
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
    asio::ip::tcp::acceptor acceptor(io_context, endpoint);
    for (;;)
    {
        asio::ip::tcp::iostream stream;
        std::error_code ec;
        acceptor.accept(stream.socket(), ec);
        if (!ec)
        {
            while (true) 
            {
                //serialize object to binary format, to variable called raw. 
                // kanske bara ska kopiera mappen till en temp map
                // frågan är hur smart det är? 
                // kanske bra så man inte blockar access till mappen för inläsning av ny data. 
                // men det kansek tar tid att kopiera skiten. 
                // kanske bara enklare att göra om alla object i mappen till binär data och lägga i en variabel. 
                if (!(stream << "hello" << std::endl)) // byt "hello", mot rätt senare. 
                {
                    std::clog << "Connection lost: " << stream.error().message() << std::endl;
                    break; 
                }
                stream.flush();
                
                // Check for any stream errors
                if (stream.fail()) 
                {
                    std::clog << "Connection error!\n"; 
                    break;  // hoppa ur och testa igen. 
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1670));  // delay
            }
        } else 
        {
            std::clog << "Failed to accept connection!\n";
        }
    }

}
*/

void send_to_client_out(std::unordered_map<int64_t, Object> &tracked_objects, std::mutex &maplock) {
    std::ostringstream outstream; //behöver fylla den här med data.  
    
    while(CONNECTION_FLAG) //.... borde egentligen avsluta den här. om connection lost men jag har trixat till det gör mig.
                // declare global variable, connection_flag, sätt 0 om max retires. connection_flag =1 om man har/försöker. 
    {
        int32_t preamble = 0xFE00;
        int32_t object_count = static_cast<int32_t>(tracked_objects.size()); //kanske det här inte "thread safe". 

        outstream.write(reinterpret_cast<const char*>(&preamble), sizeof(preamble));

        {
            //låser mappen. det blockar läsning men kanske inte gör så mycket. 
            std::scoped_lock lock(maplock);
            outstream.write(reinterpret_cast<const char*>(&object_count), sizeof(object_count));

            for (const auto &[id, obj] : tracked_objects) {
                obj.serialize(outstream);
            }
        }

        std::string data = outstream.str(); //gör om allt till en string, oklart om det är så det ska vara.

        for (unsigned char c : data) {
            std::cout << std::bitset<8>(c); //skriver ut som nollor och ettor. ska man verkligen göra så? Det här skriver bitarna: [7,6,5,4,3,2,1,0] big endian blir det.
        }
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1670)); //delay
        //det som kanske är lite märkligt är att man kommer få väldigt lite information första gången. men det kanske inte gör så mycket. 
    }
}

int main()
{
    // en map med object. borde funka ok! 
    std::unordered_map<int64_t, Object> tracked_objects;
    std::mutex mylock;
    // Det här funkar tror jag, men kanske inte är så smart/bra :)
    std::thread t1(read_from_saab, "localhost", "5463", std::ref(tracked_objects), std::ref(mylock));
    std::thread t2(send_to_client_out, std::ref(tracked_objects), std::ref(mylock));
    t1.join();
    t2.join();
    return 0;

// här kan man säkert bara sätta en timer och köra på. 
// eller köra en thread. 
// jag antar att man kör 2 thread bara. server interaction och client interaction. 
// men då behöver man thread safe access till tracked obejcts. 
// sen en tråd som hanterar att skriva till Client. 



}