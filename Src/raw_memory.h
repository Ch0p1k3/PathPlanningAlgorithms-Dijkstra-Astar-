#pragma once

#include <new>
#include <utility>

// It is a class for a implementation dynamic array
// It can allocate and dealloace raw memory

template<typename T>
class RawMemory 
{
private:
    T *buffer = nullptr;
    size_t capacity = 0;

    T* Allocate(size_t);
    void Deallocate(T*);
    void swap(RawMemory&);

public:
    RawMemory() = default;
    RawMemory(size_t);
    RawMemory(const RawMemory&) = delete;
    RawMemory(RawMemory&&);
    ~RawMemory();

    RawMemory operator=(const RawMemory&) = delete;
    RawMemory& operator=(RawMemory&&);
    T* operator+(size_t);
    const T* operator+(size_t) const;
    T& operator[](size_t);
    const T& operator[](size_t) const;

    template<typename U>
    friend class DynamicArray;
};

template<typename T>
T* RawMemory<T>::Allocate(size_t n) 
{
    return static_cast<T*>(operator new(n * sizeof(T)));
}

template<typename T>
void RawMemory<T>::Deallocate(T* buffer) 
{
    operator delete(buffer);
}

template<typename T>
RawMemory<T>::RawMemory(size_t n) 
{
    buffer = Allocate(n);
    capacity = n;
}


template<typename T>
void RawMemory<T>::swap(RawMemory<T>& other) 
{
    std::swap(buffer, other.buffer);
    std::swap(capacity, other.capacity);
}

template<typename T>
RawMemory<T>::RawMemory(RawMemory&& other)
{
    swap(other);
}

template<typename T>
T* RawMemory<T>::operator+(size_t i)
{
    return buffer + i;
}

template<typename T>
const T* RawMemory<T>::operator+(size_t i) const
{
    return buffer + i;
}

template<typename T>
T& RawMemory<T>::operator[](size_t i) 
{
    return buffer[i];
}

template<typename T>
const T& RawMemory<T>::operator[](size_t i) const
{
    return buffer[i];
}

template<typename T>
RawMemory<T>::~RawMemory() 
{
    Deallocate(buffer);
}