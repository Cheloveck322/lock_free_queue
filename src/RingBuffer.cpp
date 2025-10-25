#include "../include/RingBuffer.hpp"

template <typename T, size_t N>
RingBuffer<T, N>::RingBuffer(std::array<T, N>& arr)
    : _buffer{ std::move(arr) }, _head{ 0 }, _tail{ 0 }
{
}

template <typename T, size_t N> 
RingBuffer<T, N>::RingBuffer(std::initializer_list<T> list)
    : _buffer{}, _head{ 0 }, _tail{ 0 }
{
    size_t i{ 0 };
    for (const T& a : list)
    {
        _buffer[i++] = a;
    }
}   

template <typename T, size_t N>
bool RingBuffer<T, N>::push(const T& value)
{
    size_t head { _head.load(std::memory_order_relaxed) };
    size_t next_index{ (head + 1) % N };

    if (next_index == _tail.load(std::memory_order_acquire))
        return false; // full

    _buffer[head] = value;
    _head.store(next_index, std::memory_order_release);

    return true;
}

template <typename T, size_t N>
bool RingBuffer<T, N>::pop(T& item)
{
    size_t tail{ _tail.load(std::memory_order_relaxed) };
    size_t next_index{ (tail + 1) % N };

    if (tail == _head.load(std::memory_order_acquire))
    {
        return false;
    }
    item = _buffer[tail];
    _tail.store(next_index, std::memory_order_release);
    return true;
}