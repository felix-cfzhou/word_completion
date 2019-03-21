#pragma GCC optimize ("O2")
#pragma GCC optimize ("Ofast")

#ifndef WORDCOMPLETION
#define WORDCOMPLETION

#include<vector>
#include<string>
#include<unordered_map>
#include<iostream>
//You may add any include statements here

static bool _ = [](){
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);
    return false;
}();


using fast_t = int_fast32_t;
using idx_t = int;


struct Heap {
    struct Node {
        idx_t wordIdx;
        fast_t priority;
        Node(idx_t wordIdx, fast_t priority):
            wordIdx{wordIdx},
            priority{priority}
        {}
    };

    std::vector<Node> theHeap;
    std::unordered_map<idx_t, fast_t> &wordHeapIdxMap;

    Heap(std::unordered_map<idx_t, fast_t> &wordHeapIdxMap):
        theHeap{},
        wordHeapIdxMap{wordHeapIdxMap}
    {
        theHeap.reserve(INT8_MAX);
    }

    void fixUp(fast_t i) { // FIXME
        auto &wordHeapIdxMapEntry = wordHeapIdxMap.at(theHeap[i].wordIdx);

        for(int p=i-1 ;p>=0 && theHeap[p].priority < theHeap[i].priority; --p) {
            // update exterior pointers
            std::swap(wordHeapIdxMap.at(theHeap[p].wordIdx), wordHeapIdxMapEntry);
            std::swap(theHeap[p], theHeap[i]);
            i=p;
        }
    }

    void insert(idx_t wordIdx) {
        wordHeapIdxMap.emplace(wordIdx, theHeap.size());
        theHeap.emplace_back(wordIdx, 1);
    }
    void increasePriority(fast_t wordHeapIdx) {
        ++theHeap[wordHeapIdx].priority;
        // fixUp(wordHeapIdx);
    }

    std::vector<idx_t> kMost(fast_t k) {
        std::vector<idx_t> result;
        result.reserve(k);
        fast_t curr = 0;
        for(; curr<k && curr<static_cast<fast_t>(theHeap.size()); ++curr) result.emplace_back(theHeap[curr].wordIdx);
        for(; curr<k; ++curr) result.emplace_back(-1);

        return result;
    }
};

template<typename T, fast_t N=4096> struct FixedSizeAllocator {
    constexpr static size_t defaultSize = 2;

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
        if(nextSlot == N) {
            if(sizeBlocks == capBlocks) {
                theBlocks = static_cast<T**>(realloc(theBlocks, 2*capBlocks*sizeof(T*)));
                capBlocks *= 2;
            }   
            theBlocks[sizeBlocks++] = static_cast<T*>(malloc(N*sizeof(T)));
            nextSlot = 0;
        }   
        return &theBlocks[sizeBlocks-1][nextSlot++];
    }   

    ~FixedSizeAllocator() {
        for (fast_t k=0; k<sizeBlocks; ++k) {
            free(theBlocks[k]);
        }
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

        static void* operator new(size_t) {
            return pool.allocate();
        }

        static void operator delete(void *) noexcept {}


        Node():
            wordHeapIdxMap(INT8_MAX),
            heap{wordHeapIdxMap},
            children{
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
            wordHeapIdxMap.max_load_factor(0.5);
        }

        Node *getChild(short c) {
            return children[c-'a'];
        }

        ~Node() {
            for(short k=0; k<numChildren; ++k) delete children[k];
        }
    };

    Node* theTrie;

    Trie():
        theTrie{new Node {}}
    {}

    void access(const std::string &word, idx_t wordIdx) {
        Node *current = theTrie;
        current->heap.increasePriority(current->wordHeapIdxMap.at(wordIdx));


        for(size_t k=0; k<word.size(); ++k) {
            current = current->getChild(word[k]);
            current->heap.increasePriority(current->wordHeapIdxMap.at(wordIdx));
        } 
    }

    void insert(const std::string &word, idx_t wordIdx) {
        Node *current = theTrie;
        current->heap.insert(wordIdx);


        for(size_t k=0; k<word.size(); ++k) {
            Node *&nextNode = current->children[word[k]-'a']; 
            if(!nextNode) nextNode = new Node {};

            nextNode->heap.insert(wordIdx);

            current = nextNode;
        } 
    }

    std::vector<std::vector<idx_t>> getCompletionIdx(const std::string &word, fast_t multiplicity) {
        std::vector<std::vector<idx_t>> result;
        result.reserve(word.size() + 1);

        Node *current = theTrie;
        result.emplace_back(current->heap.kMost(multiplicity));

        size_t k = 0;
        for(; k<word.size(); ++k) {
            current = current->getChild(word[k]);
            if(!current) break;

            result.emplace_back(current->heap.kMost(multiplicity));
        }
        for(; k<word.size(); ++k) result.emplace_back(multiplicity, -1);

        return result;
    }

    ~Trie() {
        delete theTrie;
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

    std::vector<std::vector<int> > getCompletions(std::string w, int k);
    //DO NOT CHANGE THE PROVIDED INTERFACE ABOVE
};
#endif
