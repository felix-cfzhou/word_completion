#ifndef WORDCOMPLETION
#define WORDCOMPLETION

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
//You may add any include statements here

static bool _ = [](){
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);
    return false;
}();


using fast_t = int_fast32_t;
using idx_t = int;


template<typename T> struct Vector {
    T *theVector;
    fast_t theSize;
    fast_t theCap;

    Vector(fast_t initialCap):
        theVector{static_cast<T*>(malloc(initialCap*sizeof(T)))},
        theSize{0},
        theCap{initialCap}
    {}

    fast_t size() const {return theSize;}
    T &operator[](fast_t idx) {return theVector[idx];}
    const T &operator[](fast_t idx) const {return theVector[idx];}
    template<typename ...Args> void emplace_back(Args &&...args) {
        increaseCap();
        theVector[theSize++] = T(std::forward<Args>(args)...);
    }
    T &back() {return theVector[theSize-1];}
    bool empty() const {return !theSize;}

    void increaseCap() {
        if(theSize == theCap) {
            theVector = static_cast<T*>(realloc(theVector, 3*theCap*sizeof(T)));
            theCap *= 3;
        }
    }

    ~Vector() {free(theVector);}
};


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

template<typename T, fast_t N=4096> struct FixedSizeAllocator {
    // we allocate blocks of space for Trie Nodes for better cache performance
    constexpr static size_t defaultSize = 512;

    T** theBlocks;
    fast_t sizeBlocks;
    fast_t capBlocks;
    fast_t nextSlot;

    FixedSizeAllocator():
        theBlocks{static_cast<T**>(malloc(defaultSize*sizeof(T*)))},
        sizeBlocks{1},
        capBlocks{defaultSize},
        nextSlot{0}
    {   
        theBlocks[0] = static_cast<T*>(malloc(N*sizeof(T)));
    }   

    T *allocate() noexcept {
        // give our one slot
        if(nextSlot == N) {
            if(sizeBlocks == capBlocks) {
                theBlocks = static_cast<T**>(realloc(theBlocks, 3*capBlocks*sizeof(T*)));
                capBlocks *= 3;
            }   
            theBlocks[sizeBlocks++] = static_cast<T*>(malloc(N*sizeof(T)));
            nextSlot = 0;
        }   

        return &theBlocks[sizeBlocks-1][nextSlot++];
    }

    // We do not need deallocate as we never delete a Trie Node

    ~FixedSizeAllocator() {
        for (fast_t k=0; k<sizeBlocks-1; ++k) {
            for(fast_t l=0; l<N; ++l) {
                theBlocks[k][l].~T();
            }
            free(theBlocks[k]);
        }

        for(fast_t k=0; k<nextSlot; ++k) {
            theBlocks[sizeBlocks-1][k].~T();
        }
        free(theBlocks[sizeBlocks-1]);
    
        free(theBlocks);
    }
};



struct Trie {
    struct Node {
        constexpr static short numChildren = 26;

        static FixedSizeAllocator<Node> pool;

        std::unordered_map<idx_t, fast_t> wordHeapIdxMap;
        Heap heap;
        Node* children[numChildren];
        // use raw C array for child pointers

        static void* operator new(size_t) {
            return pool.allocate();
        }

        static void operator delete(void *) noexcept {}
        // we never delete so there is no need for operate delete overload


        Node():
            wordHeapIdxMap(INT8_MAX),
            heap{wordHeapIdxMap},
            children{ // explicitly construct the pointers
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
            }
        {
            wordHeapIdxMap.max_load_factor(0.4);
        }

        Node *getChild(short c) const {
            return children[c-'a'];
        }
    };

    Node* theTrie;

    Trie():
        theTrie{new Node {}}
    {}

    void access(const std::string &word, idx_t wordIdx) {
        Node *current = theTrie;
        current->heap.fixUp(current->wordHeapIdxMap.at(wordIdx));

        const size_t wordSize = word.size();

        for(size_t k=0; k<wordSize; ++k) {
            current = current->getChild(word[k]);
            current->heap.fixUp(current->wordHeapIdxMap.at(wordIdx));
        } 
    }

    void insert(const std::string &word, idx_t wordIdx) {
        Node *current = theTrie;
        current->heap.insert(wordIdx);

        const size_t wordSize = word.size();

        for(size_t k=0; k<wordSize; ++k) {
            Node *&nextNode = current->children[word[k]-'a'];
            // use a reference here so we can mutate it
            // basically a pointer to a pointer
            if(!nextNode) nextNode = new Node {};

            nextNode->heap.insert(wordIdx);

            current = nextNode;
        } 
    }

    std::vector<std::vector<idx_t>> getCompletionIdx(const std::string &word, fast_t multiplicity) {
        std::vector<std::vector<idx_t>> result;
        const size_t wordSize = word.size();
        result.reserve(wordSize + 1);

        Node *current = theTrie;
        result.emplace_back(current->heap.kMost(multiplicity));

        size_t k = 0;
        for(; k<wordSize; ++k) {
            current = current->getChild(word[k]);
            if(!current) break;

            result.emplace_back(current->heap.kMost(multiplicity));
        }
        for(; k<wordSize; ++k) result.emplace_back(multiplicity, -1);
        // ensure dimensions match

        return result;
    }
};


class wordCompletion{
    //You may add any private members you like here
    std::unordered_map<std::string, idx_t> wordIdxMap;
    Trie trie;

    public:
    //You may add any public members you like here
    //DO NOT CHANGE THE PROVIDED INTERFACE BELOW
    wordCompletion();

    int access(std::string s);

    std::vector<std::vector<int>> getCompletions(std::string w, int k);
    //DO NOT CHANGE THE PROVIDED INTERFACE ABOVE
};

#endif
