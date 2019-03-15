#ifndef WORDCOMPLETION
#define WORDCOMPLETION

#include<vector>
#include<string>
#include<unordered_map>
//You may add any include statements here


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
            std::swap(wordHeapIdxMap.at(theHeap[p].wordIdx), wordHeapIdxMap.at(theHeap[i].wordIdx));
            std::swap(theHeap[p], theHeap[i]);
            i = p;
        }
    }
    void fixDown(int i) {
        while(!isLeaf(i)) {
            int largestChildIdx = kthChild(i, 1);
            for(int k=1, j=largestChildIdx+1; k<base; ++k, ++j) {
                if(largestChildIdx == last()) break;
                else if(theHeap[j].priority > theHeap[largestChildIdx].priority) largestChildIdx = j;
            }

            if(theHeap[i].priority < theHeap[largestChildIdx].priority) {
                // updated exterior pointers
                std::swap(wordHeapIdxMap.at(theHeap[largestChildIdx].wordIdx), wordHeapIdxMap.at(theHeap[i].wordIdx));
                std::swap(theHeap[largestChildIdx], theHeap[i]);
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
        // fixDown(wordHeapIdx);
    }

    std::vector<int> kMost(int k) {
        std::vector<int> result;
        for(int curr=0; curr<k && curr<size; ++curr) result.emplace_back(theHeap[curr].wordIdx);

        return result;
    }


    ~Heap() {free(theHeap);}
};


template<typename T, size_t N=10000> struct FixedSizeAllocator {
    union Slot {
        ptrdiff_t next;
        T data;
        Slot(): next{0} {}

        ~Slot() {}
    };

    Slot theSlots[N];
    ptrdiff_t first = 0;

    FixedSizeAllocator() {
        for (size_t i = 0; i < N - 1; ++i) {
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
        int index = (static_cast<char*>(item) - reinterpret_cast<char*>(theSlots)) / sizeof(Slot);
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

        std::unordered_map<int, int> wordHeapIdxMap;
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

    void access(int wordIdx) {
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

    std::vector<std::pair<int, int>> getCompletionIdx(const std::string &word, size_t multiplicity) {
        std::vector<std::pair<int, int>> result;

        Node *current = theTrie;
        for(int idx : current->heap.kMost(multiplicity)) {
            result.emplace_back(-1, idx);
        }

        for(size_t k=0; k<word.size(); ++k) {
            current = current->getChild(word[k]);
            if(!current) {
                // std::cout << word.substr(0, k+1) << std::endl;
                break;
            }

            for(int idx : current->heap.kMost(multiplicity)) {
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
