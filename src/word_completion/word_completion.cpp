#include "word_completion.h"


using namespace std;

FixedSizeAllocator<Trie::Node> Trie::Node::pool;

WordCompletion::WordCompletion(): size{0}, trie{}
{}

idx_t WordCompletion::find(string_view s) const {
    std::shared_lock readLock {theMutex};

    const auto find_res = trie.find(s);
    return find_res.indicator == Trie::FindResult::Indicator::FOUND ? find_res.path.back()->idx : -1;
}

idx_t WordCompletion::access(string_view w) {
    // pre: w is a non-empty word with characters in {a..z}
    // post: w is added to structure if not previous in; its frequency is increased by 1
    // 	returns ID of word w
    
    std::unique_lock writeLock {theMutex};

    const auto find_result = trie.find(w);
    if(find_result.indicator != Trie::FindResult::Indicator::FOUND) {
        idx_t idx = size++;
        trie.insert(find_result, w, idx, std::move(writeLock));

        return idx;
    }

    const idx_t wordIdx = find_result.path.back()->idx;
    trie.access(find_result, wordIdx, std::move(writeLock));
    return wordIdx;
}	

vector<vector<idx_t>> WordCompletion::getCompletions(string_view w, int k) {
    // pre: Dictionary is non-empty. w is non-empty. k>=1.
    // post: see assignment for what to return
    
    std::shared_lock readLock {theMutex};

    return trie.getCompletionIdx(w, k);
}
