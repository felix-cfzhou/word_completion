#ifndef HEAP
#define HEAP

#include <vector>
#include <unordered_map>

#include "vector.h"


class Heap {
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

    public:
    Heap():
        theHeap{INT8_MAX},
        wordHeapIdxMap{INT8_MAX}
    {
    }

    Heap(const Heap &other):
        theHeap{other.theHeap},
        wordHeapIdxMap{other.wordHeapIdxMap}
    {
    }

    void fixUp(idx_t wordIdx);

    void insert(idx_t wordIdx);

    std::vector<idx_t> kMost(fast_t k) const;
};

#endif
