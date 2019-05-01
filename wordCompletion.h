#ifndef WORDCOMPLETION
#define WORDCOMPLETION

#include "trie.h"


class wordCompletion{
    //You may add any private members you like here
    std::vector<std::string> dictionary;
    std::unordered_map<std::string, idx_t> wordIdxMap;
    Trie trie;

    public:
    //You may add any public members you like here
    //DO NOT CHANGE THE PROVIDED INTERFACE BELOW
    wordCompletion();

    idx_t access(std::string s);

    std::vector<std::vector<idx_t>> getCompletions(std::string w, int k);
    //DO NOT CHANGE THE PROVIDED INTERFACE ABOVE
};

#endif
