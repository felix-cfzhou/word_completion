#ifndef WORD_COMPLETION
#define WORD_COMPLETION

#include "trie.h"


class WordCompletion{
    mutable std::shared_mutex theMutex;
    size_t size;
    Trie trie;

    public:
    WordCompletion();
    idx_t find(std::string_view s) const;

    idx_t access(std::string_view s);

    std::vector<std::vector<idx_t>> getCompletions(std::string_view w, int k);
};

#endif
