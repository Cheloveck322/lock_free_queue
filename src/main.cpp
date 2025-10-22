#include "../include/ring_buffer.hpp"
#include <iostream>
#include <thread>
#include <functional>

int main() 
{
    RingBuffer<int, 10> bf;

    for (int i{ 0 }; i < 1000; ++i)
    {
        std::thread producer{ &RingBuffer<int, 10>::push, &bf, i };
        std::thread consumer{ &RingBuffer<int, 10>::pop, &bf };

        producer.join();
        consumer.join();
    }
    for (size_t i{ 0 }; i < bf.size(); ++i)
    {
        std::cout << bf[i] << ' ';
    }
    std::cout << std::endl;

    return 0;
}