#include <cstdint>
#include <iostream>
#include <bitset>
#include <cmath> 


// det här går ju sådär. 

struct Object
{
    Object() : ID(0), X(0), Y(0), type(0){color[0]=color[1]=color[2]=0; std::cout << "object default contructor called\n";}
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



int main()
{
    int32_t preamble = 0xFE00;
    const char * ptr = reinterpret_cast<const char*>(&preamble);
    for (int i =0; i< sizeof(preamble);i++)
    {
        std::cout << std::bitset<8>(ptr[i]) << std::endl;

    }

}