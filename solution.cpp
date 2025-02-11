// hej!
#include <iostream>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <cmath>
#include <numeric>

#include <asio.hpp>


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



class Middleware {
    public:
        Middleware(asio::io_context& io_context, short client_port, std::string saab_ip, short saab_port) 
            : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), client_port)),
              client_socket_(io_context),
              saab_socket_(io_context),
              timer_(io_context, std::chrono::milliseconds(1670)) {
            
            connect_to_saab(saab_ip, saab_port);
            start_accept();
            start_timer();
        }
    
        void connect_to_saab(std::string ip, short port) {
            asio::ip::tcp::resolver resolver(saab_socket_.get_executor());
            auto endpoints = resolver.resolve(ip, std::to_string(port));
    
            asio::async_connect(saab_socket_, endpoints, [&](std::error_code ec, asio::ip::tcp::endpoint) {
                if (!ec) {
                    std::clog << "Connected to SAAB.jar\n";
                    read_from_saab();
                } else {
                    std::clog << "Failed to connect to SAAB: " << ec.message() << "\n";
                }
            });
        }
    
        void read_from_saab() {
            asio::async_read_until(saab_socket_, asio::dynamic_buffer(saab_buffer_), '\n',
                [&](std::error_code ec, std::size_t length) {
                    if (!ec) {
                        std::string line = saab_buffer_.substr(0, length);
                        saab_buffer_.erase(0, length);
                        process_incoming_data(line);
                        read_from_saab(); // Continue reading
                    } else {
                        std::clog << "SAAB read error: " << ec.message() << "\n";
                    }
                }
            );
        }
    
        void start_accept() {
            acceptor_.async_accept(client_socket_, [&](std::error_code ec) {
                if (!ec) {
                    std::clog << "Client connected\n";
                    clients_.push_back(std::move(client_socket_));
                }
                start_accept(); // Accept next client
            });
        }
    
        void start_timer() {
            timer_.async_wait([this](std::error_code ec) {
                if (!ec) {
                    send_data_to_clients();
                    timer_.expires_after(std::chrono::milliseconds(1670));
                    start_timer(); // Restart timer
                }
            });
        }
    
        void send_data_to_clients() {
            std::vector<uint8_t> binary_data = serialize_objects();
            for (auto& client : clients_) {
                asio::async_write(client, asio::buffer(binary_data), [](std::error_code, std::size_t) {});
            }
        }
    
        void process_incoming_data(const std::string& str) {
            Object obj;
            int count = sscanf(str.c_str(), "ID=%lld;X=%d;Y=%d;TYPE=%hhu", &obj.ID, &obj.X, &obj.Y, &obj.type);
            if (count == 4) {
                std::cout << str << std::endl;
                obj.set_color();
                {
                    std::scoped_lock lock(mutex_);
                    object_map_[obj.ID] = obj;
                }
            } else {
                std::clog << "Error parsing: " << str << "\n";
            }
        }
    
    private:
        std::vector<uint8_t> serialize_objects() {
            std::scoped_lock lock(mutex_);
            std::vector<uint8_t> binary_data;
            for (const auto& [id, obj] : object_map_) {
                binary_data.insert(binary_data.end(), reinterpret_cast<const uint8_t*>(&obj), 
                                   reinterpret_cast<const uint8_t*>(&obj) + sizeof(Object));
            }
            return binary_data;
        }
    
        asio::ip::tcp::acceptor acceptor_;
        asio::ip::tcp::socket client_socket_;
        asio::ip::tcp::socket saab_socket_;
        std::vector<asio::ip::tcp::socket> clients_;
        asio::steady_timer timer_;
        std::unordered_map<int64_t, Object> object_map_;
        std::mutex mutex_;
        std::string saab_buffer_;
    };
    
    int main() {
        try {
            asio::io_context io_context;
            Middleware middleware(io_context, 5000, "127.0.0.1", 5463);
            io_context.run();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
        return 0;
    }