#pragma once

#include <algorithm>
#include <iostream>
#include "dynamic_array.h"

// It's a class of Binary Heap structure. Function pop extarct a minimal element.
// Thanks for helping this conspectus: https://neerc.ifmo.ru/wiki/index.php?title=%D0%94%D0%B2%D0%BE%D0%B8%D1%87%D0%BD%D0%B0%D1%8F_%D0%BA%D1%83%D1%87%D0%B0

template<
    typename T,
    typename Container = DynamicArray<T>,
    typename Cmp = std::less<T>
> class BinaryHeap
{
private:
    Container data;
    Cmp cmp;

    void SiftDown(int i);
    void SiftUp(int i);

public:
    BinaryHeap() = default;
    BinaryHeap(const Container&, Cmp = Cmp());

    void pop();
    T top() const;
    void insert(const T&);
    bool empty() const;
};

template<
    typename T,
    typename Container,
    typename Cmp
> void BinaryHeap<T, Container, Cmp>::SiftDown(int i)
{
    while (2 * i + 1 < data.size()) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        size_t j = left;
        if (right < data.size() && cmp(data[right], data[left])) {
            j = right;
        }
        if (cmp(data[i], data[j]) || data[i] == data[j]) break;
        std::swap(data[i], data[j]);
        i = j;
    }
}

template<
    typename T,
    typename Container,
    typename Cmp
> void BinaryHeap<T, Container, Cmp>::SiftUp(int i)
{
    while (cmp(data[i], data[(i - 1) / 2])) {
        std::swap(data[i], data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

template<
    typename T,
    typename Container,
    typename Cmp
> BinaryHeap<T, Container, Cmp>::BinaryHeap(const Container& new_data, Cmp new_cmp): data(new_data), cmp(new_cmp)
{
    for (int i = data.size() / 2; i >= 0; --i) {
        SiftDown(i);
    }
}

template<
    typename T,
    typename Container,
    typename Cmp
> void BinaryHeap<T, Container, Cmp>::pop()
{
    data[0] = data[data.size() - 1];
    data.pop_back();
    SiftDown(0);
}

template<
    typename T,
    typename Container,
    typename Cmp
> T BinaryHeap<T, Container, Cmp>::top() const
{
    T res = data[0];
    return res;
}

template<
    typename T,
    typename Container,
    typename Cmp
> void BinaryHeap<T, Container, Cmp>::insert(const T& e)
{
    data.push_back(e);
    SiftUp(data.size() - 1);
}

template<
    typename T,
    typename Container,
    typename Cmp
> bool BinaryHeap<T, Container, Cmp>::empty() const
{
    return data.empty();
}