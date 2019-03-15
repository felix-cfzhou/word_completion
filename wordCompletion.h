#ifndef WORDCOMPLETION
#define WORDCOMPLETION

#include<vector>
#include<string>
#include<unordered_map>
//You may add any include statements here


using fast_t = int_fast32_t;


struct Heap {
    constexpr static fast_t defaultSize = 128; 
    constexpr static short base = 16;

    struct Node {
        fast_t wordIdx;
        fast_t priority;
        Node(fast_t wordIdx, fast_t priority):
            wordIdx{wordIdx},
            priority{priority}
        {}
    };

    Node* theHeap;
    std::unordered_map<fast_t, fast_t> &wordHeapIdxMap;
    fast_t cap;
    fast_t size;

    Heap(std::unordered_map<fast_t, fast_t> &wordHeapIdxMap):
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

    void insert(fast_t wordIdx) {
        increaseCap();
        theHeap[size] = Node(wordIdx, 1);
        wordHeapIdxMap.emplace(wordIdx, size);
        fixUp(size);

        ++size;
    }
    void increasePriority(fast_t wordHeapIdx) {
        ++theHeap[wordHeapIdx].priority;
        fixUp(wordHeapIdx);
        // fixDown(wordHeapIdx);
    }

    std::vector<fast_t> kMost(fast_t k) {
        std::vector<fast_t> result;
        for(fast_t curr=0; curr<k && curr<size; ++curr) result.emplace_back(theHeap[curr].wordIdx);

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

        std::unordered_map<fast_t, fast_t> wordHeapIdxMap;
        Heap heap;
        Node* children[numChildren];

        Node(): heap{wordHeapIdxMap} {}

        Node *&getChild(short c) {
            // std::cout << c-'a' << std::endl;
            return children[c-'a'];
        }

        ~Node() {
            for(short k=0; k<numChildren; ++k) delete children[k];
        }
    };

    const std::vector<std::string> &dictionary;
    Node* theTrie;

    Trie(const std::vector<std::string> &dictionary):
        dictionary{dictionary},
        theTrie{new Node {}}
    {}

    void access(fast_t wordIdx) {
        Node *current = theTrie;
        auto it = current->wordHeapIdxMap.find(wordIdx);
        if(it == current->wordHeapIdxMap.end()) current->heap.insert(wordIdx);
        else current->heap.increasePriority(it->second);


        const std::string &word = dictionary.at(wordIdx);
        for(size_t k=0; k<word.size(); ++k) {
            Node *&nextNode = current->getChild(word[k]); 
            if(!nextNode) nextNode = new Node {};

            it = nextNode->wordHeapIdxMap.find(wordIdx);
            if(it == nextNode->wordHeapIdxMap.end()) nextNode->heap.insert(wordIdx);
            else nextNode->heap.increasePriority(it->second);

            current = nextNode;
        } 
    }

    std::vector<std::pair<fast_t, fast_t>> getCompletionIdx(const std::string &word, fast_t multiplicity) {
        std::vector<std::pair<fast_t, fast_t>> result;

        Node *current = theTrie;
        for(fast_t idx : current->heap.kMost(multiplicity)) {
            result.emplace_back(-1, idx);
        }

        for(size_t k=0; k<word.size(); ++k) {
            current = current->getChild(word[k]);
            if(!current) break;

            for(fast_t idx : current->heap.kMost(multiplicity)) {
                result.emplace_back(k, idx);
            }
        }

        return result;
    }

    ~Trie() {delete theTrie;}
};


class wordCompletion{
    //You may add any private members you like here
    std::vector<std::string> dictionary;
    std::unordered_map<std::string, int> wordIdxMap;
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
