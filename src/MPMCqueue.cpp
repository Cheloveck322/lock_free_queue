#include "../include/MPMCqueue.hpp"

template <typename T, std::size_t N>
MPMCqueue<T, N>::MPMCqueue()
{
    for (std::size_t i = 0; i < N; ++i)
        _buffer[i].sequence.store(i, std::memory_order_relaxed);
}

template <typename T, std::size_t N>
bool MPMCqueue<T, N>::push(const T& item)
{
    std::size_t head{};
    Cell<T>* cell{ nullptr };

    for (;;)
    {
        head = _head.load(std::memory_order_relaxed);
        cell = &_buffer[head % N];
        size_t seq{ cell->sequence.load(std::memory_order_acquire) };
        int diff{ static_cast<int>(seq) - static_cast<int>(head) };

        if (diff == 0)
        {
            if (_head.compare_exchange_weak(head, head + 1, std::memory_order_release, std::memory_order_relaxed))
                break;
        }
        else if (diff < 0)
        {
            return false;
        }
        else 
        {
            head = _head.load(std::memory_order_relaxed);
        }
    }
    cell->data = item;
    cell->sequence.store(head + 1, std::memory_order_release);

    return true;
}

template <typename T, std::size_t N>
bool MPMCqueue<T, N>::pop(T& item)
{
    std::size_t tail{};
    Cell<T>* cell{ nullptr };

    for (;;)
    {
        tail = _tail.load(std::memory_order_relaxed);
        cell = &_buffer[tail % N];
        std::size_t seq{ cell->sequence.load(std::memory_order_acquire) };
        int diff{ static_cast<int>(seq) - static_cast<int>(tail + 1) };
        if (diff == 0)
        {
            if (_tail.compare_exchange_weak(tail, tail + 1, std::memory_order_release, std::memory_order_relaxed))
                break;
        }
        else if (diff < 0)
        {
            return false;
        }
        else 
        {
            tail = _tail.load(std::memory_order_relaxed);
        }
    }

    item = cell->data;
    cell->sequence.store(tail + N, std::memory_order_release);
    
    return true;
}