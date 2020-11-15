#pragma once

#include "raw_memory.h"
#include <memory>
#include <new>
#include <utility>

// class DynamicArray

template<typename T>
class DynamicArray
{
private:
    RawMemory<T> data;
    size_t sz = 0;

public:
    DynamicArray() = default;
    DynamicArray(size_t);
    DynamicArray(const DynamicArray&);
    DynamicArray(DynamicArray&&);
    ~DynamicArray();

    T* begin();
    T* end();
    const T* begin() const;
    const T* end() const;

    size_t size() const;
    size_t capacity() const;

    T& operator[](size_t);
    const T& operator[](size_t) const;
    DynamicArray& operator=(const DynamicArray&);
    DynamicArray& operator=(DynamicArray&&);

    void clear();
    void pop_back();
    void push_back(const T&);
    void push_back(T&&);
    void reserve(size_t);
    void resize(size_t);
    void swap(DynamicArray&);
};

template<typename T>
DynamicArray<T>::DynamicArray(size_t n): data(n)
{
    std::uninitialized_value_construct_n(data.buffer, n);
    sz = n;
}

template<typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other): data(other.sz)
{
    std::uninitialized_copy_n(other.data.buffer, other.sz, data.buffer);
    sz = other.size();
}

template<typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& other): data(other.sz)
{
    data.swap(other.data);
    std::swap(sz, other.sz);
}

template<typename T>
DynamicArray<T>::~DynamicArray()
{
    std::destroy_n(data.buffer, sz);
}

template<typename T>
T* DynamicArray<T>::begin()
{
    return data.buffer;
}

template<typename T>
T* DynamicArray<T>::end()
{
    return data + sz;
}

template<typename T>
const T* DynamicArray<T>::begin() const
{
    return data.buffer;
}

template<typename T>
const T* DynamicArray<T>::end() const 
{
    return data + sz;
}

template<typename T>
size_t DynamicArray<T>::size() const 
{
    return sz;
}

template<typename T>
size_t DynamicArray<T>::capacity() const
{
    return data.capacity;
}

template<typename T>
T& DynamicArray<T>::operator[](size_t i)
{
    return data[i];
}

template<typename T>
const T& DynamicArray<T>::operator[](size_t i) const
{
    return data[i];
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other)
{
    if (other.sz > data.capacity) {
        DynamicArray<T> temp(other);
        swap(temp);
    } else {
        for (size_t i = 0; i < sz && i < other.sz; ++i) {
            data[i] = other[i];
        }
        if (sz < other.sz) {
            std::uninitialized_copy_n(
                    other.data.buffer + sz,
                    other.sz - sz,
                    data.buffer + sz);
        } else if (sz > other.sz) {
            std::destroy_n(data.buffer + other.sz, sz - other.sz);
        }
        sz = other.sz;
    }
    return *this;
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& other)
{
    swap(other);
    return *this;
}

template<typename T>
void DynamicArray<T>::clear()
{
    std::destroy_n(data.buffer, sz);
    sz = 0;
}

template<typename T>
void DynamicArray<T>::pop_back()
{
    std::destroy_at(data + sz - 1);
    --sz;
}

template<typename T>
void DynamicArray<T>::push_back(const T& e)
{
    if (sz == data.capacity) {
        reserve(sz == 0 ? 1 : sz * 2);
    }
    new (data + sz)T(e);
    ++sz;
}

template<typename T>
void DynamicArray<T>::push_back(T&& e) 
{
    if (sz == data.capacity) {
        reserve(sz == 0 ? 1 : sz * 2);
    }
    new (data + sz)T(std::move(e));
    ++sz;
}

template<typename T>
void DynamicArray<T>::reserve(size_t n)
{
    if (n > data.capacity) {
        RawMemory<T> new_data(n);
        std::uninitialized_move_n(data.buffer, sz, new_data.buffer);
        std::destroy_n(data.buffer, sz);
        data.swap(new_data);
    }
}

template<typename T>
void DynamicArray<T>::resize(size_t n)
{
    reserve(n);
    if (sz < n) {
        std::uninitialized_value_construct_n(data + sz, n - sz);
    } else if (sz > n) {
        std::destroy_n(data + n, sz - n);
    }
    sz = n;
}

template<typename T>
void DynamicArray<T>::swap(DynamicArray<T>& other)
{
    data.swap(other.data);
    std::swap(sz, other.sz);
}