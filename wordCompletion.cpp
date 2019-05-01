#include "wordCompletion.h"


using namespace std;

FixedSizeAllocator<Trie::Node> Trie::Node::pool;

wordCompletion::wordCompletion():
    dictionary{},
    wordIdxMap(INT16_MAX),
    trie{}
{
    dictionary.reserve(INT16_MAX);
    wordIdxMap.max_load_factor(0.3);
}


idx_t wordCompletion::access(string w) {
    // pre: w is a non-empty word with characters in {a..z}
    // post: w is added to structure if not previous in; its frequency is increased by 1
    // 	returns ID of word w
    const auto it = wordIdxMap.find(w);
    if(it == wordIdxMap.end()) {
        idx_t idx = dictionary.size();
        wordIdxMap.emplace(w, idx);
        dictionary.emplace_back(std::move(w));

        trie.insert(dictionary.back(), idx);

        return idx;
    }

    const int idx = it->second;
    trie.access(dictionary.at(idx), it->second);

    return idx;
}	

vector<vector<idx_t>> wordCompletion::getCompletions(string w, int k) {
    // pre: Dictionary is non-empty. w is non-empty. k>=1.
    // post: see assignment for what to return

    return trie.getCompletionIdx(w, k);
}
