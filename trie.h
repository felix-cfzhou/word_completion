#ifndef TRIE
#define TRIE

#include "fixed_size_allocator.h"
#include "heap.h"


struct Trie {
    struct Node {
        constexpr static short numChildren = 26;

        static FixedSizeAllocator<Node> pool;

        std::string key;
        Heap heap;
        Node* children[numChildren];
        // use raw C array for child pointers

        static void* operator new(size_t) {
            return pool.allocate();
        }

        static void operator delete(void *) noexcept {}
        // we never delete so there is no need for operate delete overload

        Node(std::string key):
            key{std::move(key)},
            heap{},
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
        {}

        Node(std::string key, const Node &other):
            key{std::move(key)},
            heap{other.heap},
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
        {}

        Node *getChild(short c) const {
            return children[c-'a'];
        }
    };

    Node* theTrie;

    Trie():
        theTrie{new Node {""}}
    {}

    void access(std::string_view, idx_t wordIdx);

    void insert(std::string_view, idx_t wordIdx);

    std::vector<std::vector<idx_t>> getCompletionIdx(std::string_view word, fast_t multiplicity) const;
};
#endif
