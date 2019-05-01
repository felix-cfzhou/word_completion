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
    std::unordered_map<idx_t, fast_t> &wordHeapIdxMap;
    std::unordered_map<fast_t, fast_t> firstPriorityOcurrenceMap;

    Heap(std::unordered_map<idx_t, fast_t> &wordHeapIdxMap):
        theHeap{INT8_MAX},
        wordHeapIdxMap{wordHeapIdxMap},
        firstPriorityOcurrenceMap(INT8_MAX)
    {
        firstPriorityOcurrenceMap.max_load_factor(0.4);
    }

    void swap(fast_t i, fast_t swapIdx) {
        // if fixUp ruins sortedness of array, perform this wap at most once
        ++firstPriorityOcurrenceMap.at(theHeap[swapIdx].priority);
        if(swapIdx == 0 || theHeap[swapIdx-1].priority > theHeap[i].priority) firstPriorityOcurrenceMap.emplace(theHeap[i].priority, swapIdx);
        std::swap(theHeap[i], theHeap[swapIdx]);
        std::swap(wordHeapIdxMap.at(theHeap[i].wordIdx), wordHeapIdxMap.at(theHeap[swapIdx].wordIdx));
    }

    void fixUp(fast_t i) {
        // we keep track of all "runs" of priorities so we can update the priorities with at most one swap
        const fast_t oldPriority = theHeap[i].priority;
        const fast_t newPriority = ++theHeap[i].priority;

        const fast_t heapSize = theHeap.size();

        if(heapSize == 1) {
            firstPriorityOcurrenceMap.erase(oldPriority);
            firstPriorityOcurrenceMap.emplace(newPriority, 0);
        }
        else if(i == 0) {
            if(theHeap[1].priority == oldPriority) {
                ++firstPriorityOcurrenceMap.at(oldPriority);
                firstPriorityOcurrenceMap.emplace(newPriority, 0);
            }
            else {
                firstPriorityOcurrenceMap.erase(oldPriority);
                firstPriorityOcurrenceMap.emplace(newPriority, 0);
            }
        }
        else if(i == heapSize-1) {
            if(newPriority <= theHeap[i-1].priority) {
                firstPriorityOcurrenceMap.erase(oldPriority);
                if(newPriority != theHeap[i-1].priority) firstPriorityOcurrenceMap.emplace(newPriority, heapSize-1);
            }
            else swap(i, firstPriorityOcurrenceMap.at(theHeap[i-1].priority));
        }
        else {
            fast_t leftPriority = theHeap[i-1].priority;
            if(newPriority > leftPriority) swap(i, firstPriorityOcurrenceMap.at(leftPriority));
            else if(newPriority == leftPriority) {
                if(oldPriority == theHeap[i+1].priority) ++firstPriorityOcurrenceMap.at(oldPriority);
                else firstPriorityOcurrenceMap.erase(oldPriority);
            }
            else {
                firstPriorityOcurrenceMap.emplace(newPriority, i);
                if(theHeap[i+1].priority == oldPriority) ++firstPriorityOcurrenceMap.at(oldPriority);
                else firstPriorityOcurrenceMap.erase(oldPriority);
            }
        }
    }

    void insert(idx_t wordIdx) {
        wordHeapIdxMap.emplace(wordIdx, theHeap.size());
        if(theHeap.empty() || theHeap.back().priority > 1) firstPriorityOcurrenceMap.emplace(1, theHeap.size());
        theHeap.emplace_back(wordIdx, 1);
    }

    std::vector<idx_t> kMost(fast_t k) const {
        std::vector<idx_t> result;
        result.reserve(k);
        fast_t curr = 0;

        const fast_t lim = std::min(k, theHeap.size());
        for(; curr<lim; ++curr) result.emplace_back(theHeap[curr].wordIdx);
        for(; curr<k; ++curr) result.emplace_back(-1); // ensures the dimensions are correct

        return result;
    }
};

#endif
