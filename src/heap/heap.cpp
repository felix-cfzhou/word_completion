#include "heap.h"


void Heap::fixUp(idx_t wordIdx) {
    const fast_t i = wordHeapIdxMap.at(wordIdx);
    const fast_t newPriority = ++theHeap[i].priority;

    if(i == 0 || theHeap[i-1].priority >= newPriority) return;

    fast_t j = i-1;
    for(; j>=0 && theHeap[j].priority<newPriority; --j);
    std::swap(wordHeapIdxMap.at(wordIdx), wordHeapIdxMap.at(theHeap[j+1].wordIdx));
    std::swap(theHeap[i], theHeap[j+1]);
}

void Heap::insert(idx_t wordIdx) {
    wordHeapIdxMap.emplace(wordIdx, theHeap.size());
    theHeap.emplace_back(wordIdx, 1);
}

std::vector<idx_t> Heap::kMost(fast_t k) const {
    std::vector<idx_t> result;
    result.reserve(k);
    fast_t curr = 0;

    const fast_t lim = std::min(k, theHeap.size());
    for(; curr<lim; ++curr) result.emplace_back(theHeap[curr].wordIdx);
    for(; curr<k; ++curr) result.emplace_back(-1); // ensures the dimensions are correct

    return result;
}
