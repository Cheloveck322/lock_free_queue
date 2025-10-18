#include "../include/ring_buffer.hpp"
#include "ring_buffer.cpp"
#include <iostream>

int main() 
{
    RingBuffer<int, 5> cb;
    cb.push(1);
    cb.push(2);
    cb.push(3);
    std::cout << cb.pop() << std::endl; // Outputs: 1
    cb.push(4);
    cb.push(5);
    cb.push(6); // Overwrites the oldest (2)
    std::cout << cb.pop() << std::endl; // Outputs: 3

    for (int i{ 0 }; i < 5; ++i)
    {
        std::cout << cb[i] << ' ';
    }
    return 0;
}