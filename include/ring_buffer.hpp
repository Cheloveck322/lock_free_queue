#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <array>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <atomic>

template <typename T, size_t N>
class RingBuffer
{
public: 
    RingBuffer() = default;
    explicit RingBuffer(std::array<T, N>& arr);
    explicit RingBuffer(std::initializer_list<T> list);

    void push(const T& item);
    T pop();
    
    std::size_t size() const { return _buffer.size(); }
    T& operator[](size_t i) { return _buffer[i]; }

private:
    std::array<T, N> _buffer{};
    std::atomic<std::size_t> _head{ 0 };
    std::atomic<std::size_t> _tail{ 0 };

    void increase_head()
    {
        size_t next{ (_head.load(std::memory_order_acquire) + 1) % N };
        
        _head.store(next, std::memory_order_release);
    }

    void increase_tail()
    {
        size_t next{ (_tail.load(std::memory_order_acquire) + 1) % N };
        
        _tail.store(next, std::memory_order_release);
    }
};

#include "../src/ring_buffer.cpp"

#endif