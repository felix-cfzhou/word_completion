#pragma GCC optimize ("O3")
#pragma GCC optimize ("Ofast")

#ifndef WORDCOMPLETION
#define WORDCOMPLETION

#include<vector>
#include<string>
#include<unordered_map>
//You may add any include statements here


using fast_t = int;
using idx_t = int;


struct Heap {
    constexpr static fast_t defaultSize = 8; 

    struct Node {
        idx_t wordIdx;
        fast_t priority;
        Node(idx_t wordIdx, fast_t priority):
            wordIdx{wordIdx},
            priority{priority}
        {}
    };

    Node* theHeap;
    std::unordered_map<idx_t, fast_t> &wordHeapIdxMap;
    fast_t cap;
    fast_t size;

    Heap(std::unordered_map<idx_t, fast_t> &wordHeapIdxMap):
        theHeap{static_cast<Node*>(malloc(defaultSize * sizeof(Node)))},
        wordHeapIdxMap{wordHeapIdxMap},
        cap{defaultSize},
        size{0}
    {}

    void increaseCap() {
        if(size == cap) {
            theHeap = static_cast<Node*>(realloc(theHeap, 2*cap*sizeof(Node)));
            cap *= 2;
        }
    }

    void fixUp(fast_t i) {
        auto &wordHeapIdxMapEntry = wordHeapIdxMap.at(theHeap[i].wordIdx);

        for(int p=i-1 ;p>=0 && theHeap[p].priority < theHeap[i].priority; --p) {
            // update exterior pointers
            std::swap(wordHeapIdxMap.at(theHeap[p].wordIdx), wordHeapIdxMapEntry);
            std::swap(theHeap[p], theHeap[i]);
            i=p;
        }
    }

    void insert(idx_t wordIdx) {
        increaseCap();
        theHeap[size] = Node(wordIdx, 1);
        wordHeapIdxMap.emplace(wordIdx, size++);
    }
    void increasePriority(fast_t wordHeapIdx) {
        ++theHeap[wordHeapIdx].priority;
        fixUp(wordHeapIdx);
    }

    std::vector<idx_t> kMost(fast_t k) {
        std::vector<idx_t> result;
        result.reserve(k);
        fast_t curr = 0;
        for(; curr<k && curr<size; ++curr) result.emplace_back(theHeap[curr].wordIdx);
        for(; curr<k; ++curr) result.emplace_back(-1);
        for(fast_t l=0; l<size; ++l) std::cout << theHeap[l].priority << ' ' << theHeap[l].wordIdx << std::endl;
        std::cout << "---------------" << std::endl;

        return result;
    }


    ~Heap() {free(theHeap);}
};


template<typename T, fast_t N=10> struct FixedSizeAllocator {
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
        for (fast_t k=0; k<sizeBlocks-1; ++k) {
            for(fast_t l=0; l<N; ++l) {
                theBlocks[k][l].~T();
            }
            free(theBlocks[k]);
        }
        for(fast_t l=0; l<nextSlot; ++l) {
            theBlocks[sizeBlocks-1][l].~T();
        }
        free(theBlocks[sizeBlocks-1]);
        free(theBlocks);
    }
};

struct Trie {
    struct Node {
        constexpr static short numChildren = 26;
        static FixedSizeAllocator<Node> pool;

        static void* operator new(size_t) {
            return pool.allocate();
        }

        static void operator delete(void *) noexcept {}

        std::unordered_map<idx_t, fast_t> wordHeapIdxMap;
        Heap heap;
        Node* children[numChildren];

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

        Node *&getChild(short c) {
            // std::cout << c-'a' << std::endl;
            return children[c-'a'];
        }

        ~Node() {
            // for(short k=0; k<numChildren; ++k) delete children[k];
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
            Node *&nextNode = current->getChild(word[k]); 
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
        // delete theTrie;
    }
};


class wordCompletion{
    //You may add any private members you like here
    std::unordered_map<std::string, idx_t> wordIdxMap;
    Trie trie;
    fast_t dicSize;


    public:
    //You may add any public members you like here



    //DO NOT CHANGE THE PROVIDED INTERFACE BELOW
    wordCompletion();

    int access(std::string s);

    std::vector<std::vector<int> > getCompletions(std::string w, int k);
    //DO NOT CHANGE THE PROVIDED INTERFACE ABOVE
};
#endif
