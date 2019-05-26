#ifndef TRIE
#define TRIE

#include <shared_mutex>

#include "fixed_size_allocator.h"
#include "heap.h"


class Trie {
    struct Node {
        constexpr static short numChildren = 128;

        static FixedSizeAllocator<Node> pool;

        mutable std::shared_mutex theMutex;
        std::string key;
        idx_t idx;
        Heap heap;
        Node* children[numChildren];
        // use raw C array for child pointers

        static void* operator new(size_t) {
            return pool.allocate();
        }

        static void operator delete(void *) noexcept {}
        // we never delete so there is no need for operate delete overload

        [[ using gnu : hot ]]
        Node(std::string key, idx_t idx);

        [[ using gnu : hot ]]
        Node(std::string key, idx_t idx, const Node &other);

        Node(const Node &other) = delete;

        Node *&operator[](short c) {
            return children[c];
        }
    };

    Node* theTrie;

    void displayTrie(Node *, size_t) const;

    public:
    struct FindResult {
        using Path = std::vector<Node *>;

        enum class Indicator {
            FOUND = 0,
            END_OF_TRIE,
            END_OF_NEEDLE,
            SPLIT,
            FOUND_SPLIT,
        };

        const Indicator indicator;
        const Path path;
        const size_t needleCharIdx;
        const size_t keyCharIdx;

        FindResult(Indicator indicator, std::vector<Node *> path, size_t needleCharIdx, size_t keyCharIdx):
            indicator{indicator},
            path{std::move(path)},
            needleCharIdx{needleCharIdx},
            keyCharIdx{keyCharIdx}
        {}
    };

    Trie():
        theTrie{new Node {"", -1}}
    {}
    Trie(const Trie &other) = delete;
    Trie &operator=(const Trie &other) = delete;

    const FindResult find(std::string_view) const;

    [[ deprecated ]]
    void access(std::string_view, idx_t wordIdx);
    void access(const FindResult &findResult, idx_t wordIdx);

    [[ deprecated ]]
    void insert(std::string_view, idx_t wordIdx);
    void insert(const FindResult &findResult, std::string_view word, idx_t wordIdx);

    std::vector<std::vector<idx_t>> getCompletionIdx(std::string_view word, fast_t multiplicity) const;
};

#endif
