#include "word_completion.h"


using namespace std;

FixedSizeAllocator<Trie::Node> Trie::Node::pool;

WordCompletion::WordCompletion():
    dictionary{},
    trie{}
{
    dictionary.reserve(INT16_MAX);
}

idx_t WordCompletion::find(string_view s) const {
    const auto find_res = trie.find(s);
    return find_res.indicator == Trie::FindResult::Indicator::FOUND ? find_res.path.back()->idx : -1;
}

idx_t WordCompletion::access(string w) {
    // pre: w is a non-empty word with characters in {a..z}
    // post: w is added to structure if not previous in; its frequency is increased by 1
    // 	returns ID of word w
    const auto find_result = trie.find(w);
    if(find_result.indicator != Trie::FindResult::Indicator::FOUND) {
        idx_t idx = dictionary.size();
        dictionary.emplace_back(std::move(w));

        trie.insert(dictionary.back(), idx);

        return idx;
    }

    const idx_t wordIdx = find_result.path.back()->idx;
    trie.access(find_result.path, wordIdx);
    return wordIdx;
}	

vector<vector<idx_t>> WordCompletion::getCompletions(string w, int k) {
    // pre: Dictionary is non-empty. w is non-empty. k>=1.
    // post: see assignment for what to return

    return trie.getCompletionIdx(w, k);
}
