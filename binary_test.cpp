#include <cstdint>
#include <iostream>
#include <bitset>


// det här går ju sådär. 

int main()
{
    // uint8_t test = 0x00;
    int64_t test = 1;
    const char* binary_test = reinterpret_cast<const char*>(&test);
    std::cout << std::bitset<8>(binary_test[sizeof(binary_test)-1]) << std::endl;
}