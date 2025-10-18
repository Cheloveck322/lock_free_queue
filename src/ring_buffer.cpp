#include "../include/ring_buffer.hpp"

template <typename T, size_t N>
RingBuffer<T, N>::RingBuffer(std::array<T, N>& arr)
    : _buffer{ std::move(arr) }, _head{ 0 }, _tail{ N - 1 }
{
}

template <typename T, size_t N> 
RingBuffer<T, N>::RingBuffer(std::initializer_list<T> list)
    : _buffer(N), _head{ 0 }, _tail{ N - 1 }
{
    for (const T& a : list)
    {
        size_t i{ 0 };
        _buffer[i++] = a;
    }
}

template <typename T, size_t N>
void RingBuffer<T, N>::push(const T& value)
{
    
}

template <typename T, size_t N>
T RingBuffer<T, N>::pop()
{
    
}