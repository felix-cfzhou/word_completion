#ifndef TRIE
#define TRIE

#include "fixed_size_allocator.h"
#include "heap.h"


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

    void access(std::string_view word, idx_t wordIdx) {
        Node *current = theTrie;
        current->heap.fixUp(current->wordHeapIdxMap.at(wordIdx));

        const size_t wordSize = word.size();

        for(size_t k=0; k<wordSize; ++k) {
            current = current->getChild(word[k]);
            current->heap.fixUp(current->wordHeapIdxMap.at(wordIdx));
        } 
    }

    void insert(std::string_view word, idx_t wordIdx) {
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

    std::vector<std::vector<idx_t>> getCompletionIdx(std::string_view word, fast_t multiplicity) {
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
#endif
