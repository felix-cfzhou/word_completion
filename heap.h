#include <utility>
#include <vector>
#include<unordered_map>
#include <stdlib.h>


struct Heap {
    constexpr static int defaultSize = 64; 
    constexpr static short base = 16;

    struct Node {
        int wordIdx;
        size_t priority;
        Node(int wordIdx, size_t priority):
            wordIdx{wordIdx},
            priority{priority}
            {}
    };

    Node* theHeap;
    std::unordered_map<int, int> &wordHeapIdxMap;
    int cap;
    int size;

    Heap(std::unordered_map<int, int> &wordHeapIdxMap):
        theHeap{static_cast<Node*>(malloc(defaultSize * sizeof(Node)))},
        wordHeapIdxMap{wordHeapIdxMap},
        cap{defaultSize},
        size{0}
    {}
    
    int root() {return 0;}
    int kthChild(int i, short k) {return base*i+k;}
    int parent(size_t i) {return (i-1)/base;}
    int last() {return size-1;}
    bool isLeaf(size_t i) {return kthChild(i, 1) >= size;}
    void increaseCap() {
        if(size == cap) {
            theHeap = static_cast<Node*>(realloc(theHeap, 2*cap*sizeof(Node)));
            cap *= 2;
        }
    }

    void fixUp(int i) {
        for(int p = parent(i); p>=0 && theHeap[p].priority < theHeap[i].priority; p=parent(i)) {
            // update exterior pointers
            std::swap(theHeap[p], theHeap[i]);
            std::swap(wordHeapIdxMap[theHeap[p].wordIdx], wordHeapIdxMap[theHeap[i].wordIdx]);
            i = p;
        }
    }
    void fixDown(int i) {
        while(!isLeaf(i)) {
            int largestChildIdx = kthChild(i, 1);
            for(int k=1, j=largestChildIdx; k<base; ++k, ++j) {
                if(largestChildIdx == last()) break;
                else if(theHeap[j].priority > theHeap[largestChildIdx].priority) largestChildIdx = j;
            }

            if(theHeap[i].priority < theHeap[largestChildIdx].priority) {
                // updated exterior pointers
                std::swap(theHeap[largestChildIdx], theHeap[i]);
                std::swap(wordHeapIdxMap[theHeap[largestChildIdx].wordIdx], wordHeapIdxMap[theHeap[i].wordIdx]);
                i = largestChildIdx;
            }
            else break;
        }
    }

    void insert(int wordIdx) {
        increaseCap();
        theHeap[size] = Node(wordIdx, 1);
        wordHeapIdxMap.emplace(wordIdx, size);
        fixUp(size);

        ++size;
    }
    void increasePriority(int wordHeapIdx) {
        ++theHeap[wordHeapIdx].priority;
        fixUp(wordHeapIdx);
        fixDown(wordHeapIdx);
    }

    std::vector<int> kMost(int k) {
        std::vector<int> result;
        for(int curr=0; curr<k; ++curr) result.emplace_back(theHeap[curr].wordIdx);

        return result;
    }


    ~Heap() {free(theHeap);}
};
