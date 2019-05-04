#ifndef WORD_COMPLETION
#define WORD_COMPLETION

#include "trie.h"


class WordCompletion{
    std::vector<std::string> dictionary;
    Trie trie;

    public:
    WordCompletion();
    idx_t find(std::string_view s) const;

    idx_t access(std::string s);

    std::vector<std::vector<idx_t>> getCompletions(std::string w, int k);
};

#endif
