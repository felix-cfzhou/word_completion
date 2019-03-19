#ifndef WORDCOMPLETION
#define WORDCOMPLETION

#include<vector>
#include<string>
#include<unordered_map>
//You may add any include statements here


using fast_t = int_fast32_t;
using idx_t = int;


struct Heap {
    constexpr static fast_t defaultSize = 128; 
    constexpr static short base = 16;

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

    int root() {return 0;}
    int kthChild(fast_t i, short k) {return base*i+k;}
    int parent(fast_t i) {return (i-1)/base;}
    int last() {return size-1;}
    bool isLeaf(fast_t i) {return kthChild(i, 1) >= size;}
    void increaseCap() {
        if(size == cap) {
            theHeap = static_cast<Node*>(realloc(theHeap, 2*cap*sizeof(Node)));
            cap *= 2;
        }
    }

    void fixUp(fast_t i) {
        auto &wordHeapIdxMapEntry = wordHeapIdxMap.at(theHeap[i].wordIdx);

        for(int p=parent(i) ;p>=0 && theHeap[p].priority < theHeap[i].priority; p=parent(i)) {
            // update exterior pointers
            std::swap(wordHeapIdxMap.at(theHeap[p].wordIdx), wordHeapIdxMapEntry);
            std::swap(theHeap[p], theHeap[i]);
            i = p;
        }
    }

    void insert(idx_t wordIdx) {
        increaseCap();
        theHeap[size] = Node(wordIdx, 1);
        wordHeapIdxMap.emplace(wordIdx, size);
        fixUp(size);

        ++size;
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

        return result;
    }


    ~Heap() {free(theHeap);}
};


template<typename T, fast_t N=7000000> struct FixedSizeAllocator {
    union Slot {
        fast_t next;
        T data;
        Slot(): next{0} {}

        ~Slot() {}
    };

    Slot theSlots[N];
    fast_t first = 0;

    FixedSizeAllocator() {
        for (fast_t i = 0; i < N - 1; ++i) {
            theSlots[i].next = i + 1;
        }
        theSlots[N - 1].next = -1;
    }

    T *allocate() noexcept {
        if (first == -1) return nullptr;
        T *result = &(theSlots[first].data);
        first = theSlots[first].next;
        return result;
    }

    void deallocate(void *item) noexcept {
        fast_t index = (static_cast<char*>(item) - reinterpret_cast<char*>(theSlots)) / sizeof(Slot);
        theSlots[index].next = first;
        first = index;
    }

    ~FixedSizeAllocator() {}
};

struct Trie {
    struct Node {
        constexpr static short numChildren = 26;
        static FixedSizeAllocator<Node> pool;

        static void* operator new(size_t size) {
            if (size != sizeof(Node)) throw std::bad_alloc{};
            while (true) {
                void *p = pool.allocate();
                if (p) return p;
                std::new_handler h = std::set_new_handler(0);
                std::set_new_handler(h);
                if (h) h();
                else throw std::bad_alloc{};
            }
        }

        static void operator delete(void *p) noexcept {
            if (p == nullptr) return;
            pool.deallocate(p);
        }

        std::unordered_map<idx_t, fast_t> wordHeapIdxMap;
        Heap heap;
        Node* children[numChildren];

        Node(): heap{wordHeapIdxMap} {}

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
            Node *&nextNode = current->getChild(word[k]); 
            if(!nextNode) nextNode = new Node {};

            nextNode->heap.insert(current->wordHeapIdxMap.at(wordIdx));

            current = nextNode;
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
