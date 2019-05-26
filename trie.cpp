#include "trie.h"


Trie::Node::Node(std::string key, idx_t idx):
    key{std::move(key)},
    idx{idx},
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

Trie::Node::Node(std::string key, idx_t idx, const Node &other):
    key{std::move(key)},
    idx{idx},
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

const Trie::FindResult Trie::find(std::string_view word) const {
    std::vector<Node *> path {theTrie};
    Node *current = theTrie;

    const size_t wordSize = word.size();
    for(size_t k=0; k<wordSize;) {
        current = (*current)[word[k]];
        if(!current) return {FindResult::Indicator::END_OF_TRIE, std::move(path), k, 0};
        path.emplace_back(current);

        const size_t keySize = current->key.size();
        for(size_t l=0; l<keySize; ++l, ++k) {
            if(k == wordSize)return {FindResult::Indicator::END_OF_NEEDLE, std::move(path), k, l};
            else if(word[k] != current->key[l]) return {FindResult::Indicator::SPLIT, std::move(path), k, l};
        }

        if(k == wordSize) {
            if(current->idx < 0) return {FindResult::Indicator::FOUND_SPLIT, std::move(path), k, 0};
            else return {FindResult::Indicator::FOUND, std::move(path), k, 0};
        }
    } 

    return {FindResult::Indicator::END_OF_TRIE, std::move(path), 0, 0};
}

void Trie::access(const FindResult &findResult, idx_t wordIdx) {
    for(auto nodePtr : findResult.path) nodePtr->heap.fixUp(wordIdx);
}

void Trie::insert(const Trie::FindResult &findResult, std::string_view word, idx_t wordIdx) {
    // std::cout << "inserting: " << word << std::endl;
    const auto &path = findResult.path;
    const size_t pathSize = path.size();
    for(size_t k=0; k<pathSize-1; ++k) path[k]->heap.insert(wordIdx);

    Node *last = path.back();
    switch(findResult.indicator) {
        case FindResult::Indicator::END_OF_TRIE:
            {
                last->heap.insert(wordIdx);
                auto &child = (*last)[word[findResult.needleCharIdx]];
                child = new Node{std::string(word.substr(findResult.needleCharIdx)), wordIdx};
                child->heap.insert(wordIdx);
            }
            break;
        case FindResult::Indicator::END_OF_NEEDLE:
            {
                Node &parent = *path[path.size()-2];
                Node *child = new Node {last->key.substr(0, findResult.keyCharIdx), wordIdx, *last};
                parent[child->key.front()] = child;
                child->heap.insert(wordIdx);
                last->key.erase(0, findResult.keyCharIdx);
                (*child)[last->key.front()] = last;
            }
            break;
        case FindResult::Indicator::SPLIT:
            {
                Node &parent = *path[path.size()-2];
                Node *newGrandChild = new Node{std::string(word.substr(findResult.needleCharIdx)), wordIdx};
                newGrandChild->heap.insert(wordIdx);
                Node *child = new Node{last->key.substr(0, findResult.keyCharIdx), -1, *last};
                parent[child->key.front()] = child;
                child->heap.insert(wordIdx);
                last->key.erase(0, findResult.keyCharIdx);
                (*child)[last->key.front()] = last;
                (*child)[newGrandChild->key.front()] = newGrandChild;
            }
            break;
        case FindResult::Indicator::FOUND_SPLIT:
            {
                last->idx = wordIdx;
                last->heap.insert(wordIdx);
            }
            break;
        default: throw std::logic_error {"illegal insert, word already exists"};
    }
    // displayTrie(theTrie);
    // std::cout << std::endl;
}

std::vector<std::vector<idx_t>> Trie::getCompletionIdx(std::string_view word, fast_t multiplicity) const {
    std::vector<std::vector<idx_t>> result;
    const size_t wordSize = word.size();
    result.reserve(wordSize + 1);

    Node *current = theTrie;
    result.emplace_back(current->heap.kMost(multiplicity));

    for(size_t idx=0; idx<wordSize;) {
        current = (*current)[word[idx]];
        if(!current) break;

        const auto temp = current->heap.kMost(multiplicity);
        const std::string_view &currentKey = current->key;
        const size_t keySize = currentKey.size();

        for(size_t k=0; k<keySize; ++k, ++idx) {
            if(idx==wordSize || word[idx]!=currentKey[k]) goto ENDGETCOMPLETIONIDX;
            result.emplace_back(temp);
        }
    }

    ENDGETCOMPLETIONIDX:
    for(size_t k=result.size(); k<wordSize+1; ++k) result.emplace_back(multiplicity, -1);
    // ensure dimensions match

    return result;
}
