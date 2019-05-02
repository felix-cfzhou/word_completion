#ifndef HEAP
#define HEAP

#include <vector>
#include <unordered_map>

#include "vector.h"


struct Heap {
    // note this is actually a sorted array
    // but it is easier to think of its role as a max heap
    struct Node {
        idx_t wordIdx;
        fast_t priority;
        Node(idx_t wordIdx, fast_t priority):
            wordIdx{wordIdx},
            priority{priority}
        {}
    };

    Vector<Node> theHeap;
    std::unordered_map<idx_t, fast_t> wordHeapIdxMap;
    std::unordered_map<fast_t, fast_t> firstPriorityOcurrenceMap;

    Heap():
        theHeap{INT8_MAX},
        wordHeapIdxMap{INT8_MAX},
        firstPriorityOcurrenceMap(INT8_MAX)
    {
        firstPriorityOcurrenceMap.max_load_factor(0.4);
    }

    Heap(const Heap &other):
        theHeap{other.theHeap},
        wordHeapIdxMap{other.wordHeapIdxMap},
        firstPriorityOcurrenceMap(other.firstPriorityOcurrenceMap)
    {
        firstPriorityOcurrenceMap.max_load_factor(0.4);
    }

    void swap(fast_t i, fast_t swapIdx);

    void fixUp(fast_t i);

    void insert(idx_t wordIdx);

    std::vector<idx_t> kMost(fast_t k) const;
};

#endif
