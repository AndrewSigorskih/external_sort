#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <queue>
#include <vector>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

namespace FS = std::filesystem;

const int DIRNAME_LEN = 10;

void create_random_dir(char* buf, int len, uint n_tries = 100);

template<class T>
struct QueueNode
{
    uint64_t m_index;
    T m_data;
    QueueNode (uint64_t i, T item) : m_index(i), m_data(std::move(item)) {}
    // invert the sign so that max_heap turns into min_heap
    bool operator<(const QueueNode& other) const {
        return (this->m_data > other.m_data);
    }

    const T& data() const { return this->m_data; } 
};

template <class T, int LinesPerObj = 1>
class ExternalSorter
{
private:
    uint64_t m_batchsize, m_filesNum;
    char m_tempdir[DIRNAME_LEN + 1];
    std::priority_queue<QueueNode<T>, std::vector<QueueNode<T>>> m_queue;
public:
    ExternalSorter(uint64_t batch) : m_batchsize(batch)
    {
        m_tempdir[DIRNAME_LEN] = '\0';
        std::vector<QueueNode<T>> container;
        container.reserve(m_batchsize);
        m_queue = std::priority_queue<QueueNode<T>, std::vector<QueueNode<T>>>
            (std::less<QueueNode<T>>(), std::move(container));
    }
    void sort(const char*);
    void mergeHelper(uint64_t, uint64_t, uint64_t);
    void merge(const char*);
    void operator()(const char* infilename, const char* outfilename);
};

#include "external_sort.tpp"
