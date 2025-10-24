#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <array>
#include <initializer_list>
#include <utility>
#include <atomic>

template <typename T, size_t N>
class RingBuffer
{
public: 
    RingBuffer() = default;
    explicit RingBuffer(std::array<T, N>& arr);
    explicit RingBuffer(std::initializer_list<T> list);

    bool push(const T& item);
    bool pop();
    
    std::size_t size() const { return _buffer.size(); }
    T& operator[](size_t i) { return _buffer[i]; }

private:
    std::array<T, N> _buffer{};
    std::atomic<std::size_t> _head{ 0 };
    std::atomic<std::size_t> _tail{ 0 };
};

#include "../src/ring_buffer.cpp"

#endif