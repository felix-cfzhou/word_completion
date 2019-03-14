#pragma GCC optimize ("O3")

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <new>
#include <unordered_map>


static bool _ = [](){
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);
    return 0;
}();


struct Heap {
    constexpr static int defaultSize = 128; 
    constexpr static short base = 16;

    struct Node {
        size_t wordIdx;
        size_t priority;
        Node(size_t wordIdx, size_t priority):
            wordIdx{wordIdx},
            priority{priority}
            {}
    };

    Node* theHeap;
    size_t cap;
    size_t size;

    size_t root() {return 0;}
    size_t kthChild(size_t i, short k) {return base*i+k;}
    ptrdiff_t parent(size_t i) {return (i-1)/base;}
    size_t last() {return size-1;}
    bool isLeaf(size_t i) {return kthChild(i, 1) >= size;}
    void increaseCap() {
        if(size == cap) {
            theHeap = static_cast<Node*>(realloc(theHeap, 2*cap*sizeof(Node)));
            cap *= 2;
        }
    }

    Heap():
        theHeap{static_cast<Node*>(malloc(defaultSize * sizeof(Node)))},
        cap{defaultSize},
        size{0}
    {}
    
    void fixUp(size_t i) {
        for(auto p = parent(i); p>=0 && theHeap[p].priority < theHeap[i].priority; p=parent(i)) {
            // update exterior pointers
            std::swap(theHeap[p], theHeap[i]);
            i = p;
        }
    }
    void fixDown(size_t i) {
        while(!isLeaf(i)) {
            auto j = kthChild(i, 1);
            for(size_t k=0; k<base; ++k) {
                if(j == last()) break;
                else if(theHeap[j+1].priority > theHeap[j].priority) j += 1;
            }

            if(theHeap[i].priority < theHeap[j].priority) {
                // updated exterior pointers
                std::swap(theHeap[j], theHeap[i]);
                i = j;
            }
            else break;
        }
    }

    void insert(size_t wordIdx, size_t priority) {
        increaseCap();
        theHeap[size] = Node(wordIdx, priority);
        fixUp(size);

        ++size;
    }
    void updatePriority(size_t i, size_t newPriority) {
        theHeap[i].priority = newPriority;
        fixUp(i);
        fixDown(i);
    }
    std::vector<size_t> kMost(size_t k) {
        std::vector<size_t> stack {0};
        std::vector<size_t> result;

        if(k == 0) return result;

        while(k > 0 && !stack.empty()) {
            size_t i = stack.back();
            result.emplace_back(theHeap[i].wordIdx);
            --k;

            for(size_t j=kthChild(i, 1), count=0; count<base; ++j, ++count) {
                if(j == size) break;
                stack.emplace_back(j);
            }
        }

        return result;
    }


    ~Heap() {free(theHeap);}
};

template<typename T, size_t N> class FixedSizeAllocator {
    private:
        union Slot {
            ptrdiff_t next;
            T data;
            Slot(): next{0} {}
        };

        Slot theSlots[N];
        ptrdiff_t first = 0;
    public:
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
};

struct Trie {
    struct Node {
        constexpr static short numChildren = 26;
        static FixedSizeAllocator<Node, UINT32_MAX> pool;

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

        Heap completions;
        std::unordered_map<size_t, size_t> wordIdxMap;
        Node* children[numChildren];

        Node() {}
        

        Node *&getChild(short c) {return children[c];}

        ~Node() {
            for(short k=0; k<numChildren; ++k) delete children[k];
        }
    };

    Node* theTrie = new Node{};

    void insert(const std::string &word, size_t multiplicity) {
        Node* current = theTrie;
        for(size_t k=0; k<word.size(); ++k) {
            Node *&nextNode = current->getChild(word[k]); 
            if(!nextNode) nextNode = new Node {};
                
            size_t dictionaryIdx = 0; // FIXME
            auto it = nextNode->wordIdxMap.find(dictionaryIdx);
            if(it == nextNode->wordIdxMap.end()) {
                nextNode->completions.insert(dictionaryIdx, multiplicity);
            }
            else {
                nextNode->completions.updatePriority(it->second, nextNode->completions.theHeap[it->second].priority+multiplicity);
            }
        } 
    }

    std::vector<std::pair<int, int>> getCompletionIdx(const std::string &word, size_t multiplicity) {
        std::vector<std::pair<int, int>> result;

        Node *current = theTrie;
        for(size_t k=0; k<word.size(); ++k) {
            current = current->getChild(word[k]);
            if(!current) break;

            for(int res: current->completions.kMost(multiplicity)) {
                result.emplace_back(k, res);
            }
        }

        return result;
    }

    ~Trie() {delete theTrie;}
};

class wordCompletion {
    std::vector<std::string> dictionary;
    std::unordered_map<std::string, int> wordIdxMap;
    Trie trie;

    int access(std::string word) {
        auto it = wordIdxMap.find(word);

        if(it == wordIdxMap.end()) {
            dictionary.emplace_back(std::move(word));

            return dictionary.size()-1;
        }
        else return it->second;
    }

    std::vector<std::vector<int>> getCompletions(std::string word, int k) {
        std::vector<std::vector<int>> result;
        result.reserve(k);

        for(auto res : trie.getCompletionIdx(word, k)) {
            result[res.first].emplace_back(res.second);
        }
        for(int idx=0; idx<k; ++idx) {
            while(static_cast<int>(result[idx].size()) < k) result[idx].emplace_back(-1);
        }

        return result;
    }
};
