#include "../include/ring_buffer.hpp"

template <typename T, size_t N>
RingBuffer<T, N>::RingBuffer(std::array<T, N>& arr)
    : _buffer{ std::move(arr) }, _head{ 0 }, _tail{ 0 }
{
}

template <typename T, size_t N> 
RingBuffer<T, N>::RingBuffer(std::initializer_list<T> list)
    : _buffer{}, _head{ 0 }, _tail{ 0 }
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
    size_t next_index{ (_head + 1) % N };
    bool is_full{ next_index == _tail.load(std::memory_order_relaxed) };

    if (is_full)
    {
        increase_tail();
    }

    _buffer[_head] = value;
    increase_head();
}

template <typename T, size_t N>
T RingBuffer<T, N>::pop()
{
    while (_head.load(std::memory_order_seq_cst) == _tail.load(std::memory_order_relaxed)); // wait while they are same 

    size_t curr_index{ _tail.load(std::memory_order_relaxed) };
    increase_tail();
    return _buffer[curr_index];
}