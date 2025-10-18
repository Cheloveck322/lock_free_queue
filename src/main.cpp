#include "../include/ring_buffer.hpp"
#include <iostream>

int main()
{
    RingBuffer buff{ 1, 2, 3, 4, 5 };

    buff.push_back(123);

    for (size_t i{ 0 }; i < buff.size(); ++i)
    {
        std::cout << buff[i] << ' ';
    }

    return 0;
}