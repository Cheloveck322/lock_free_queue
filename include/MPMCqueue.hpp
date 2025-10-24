#ifndef MPMCQUEUE_HPP
#define MPMCQUEUE_HPP

#include <cstddef>
#include <atomic>
#include <array>

template <typename T>
struct Cell
{
    std::atomic<std::size_t> sequence;
    T data;
};

template <typename T, std::size_t N>
class MPMCqueue
{
    public:
    MPMCqueue();
    
    bool push(const T& item);
    bool pop(T& item);
    
    private:
    std::array<Cell<T>, N> _buffer{};
    std::atomic<std::size_t> _head{};
    std::atomic<std::size_t> _tail{};
};

#include "../src/MPMCqueue.cpp"

#endif