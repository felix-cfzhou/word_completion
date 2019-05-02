#include <cassert>
#include <iostream>

#include "trie.h"


void Trie::access(std::string_view word, idx_t wordIdx) {
    // std::cout << "accessing: " << word << std::endl;
    Node *current = theTrie;
    current->heap.fixUp(current->heap.wordHeapIdxMap.at(wordIdx));

    const size_t wordSize = word.size();
    for(size_t k=0; k<wordSize;) {
        // std::cout << current->key << ':' << std::endl;
        // for(auto child : current->children) if(child) std::cout << child->key << std::endl;
        current = current->getChild(word[k]);
        current->heap.fixUp(current->heap.wordHeapIdxMap.at(wordIdx));

        k += current->key.size();
    } 
}

void displayTrie(Trie::Node *trie, size_t indent=0) {
    for(size_t count=0; count < indent; ++count) std::cout << "  ";
    std::cout << trie->key << ": ";
    for(auto child : trie->children) {
        if(child) std::cout << child->key;
        std::cout << ',';
    }
    std::cout << std::endl;

    for(auto child : trie->children) {
        if(child) displayTrie(child, indent+1);
    }
}

void Trie::insert(std::string_view word, idx_t wordIdx) {
    // std::cout << "inserting: " << word << std::endl;

    Node *parent = theTrie;
    parent->heap.insert(wordIdx);

    const size_t wordSize = word.size();
    for(size_t k=0; k<wordSize;) {
        Node *&current = parent->children[word[k] - 'a'];
        if(!current) {
            current = new Node{std::string(word.substr(k))};
            // std::cout << parent->key << std::endl;
            // std::cout << parent->getChild(word[k])->key << std::endl;
            current->heap.insert(wordIdx);
            // std::cout << "word long" << std::endl;
            break;
        }

        const size_t keySize = current->key.size();
        for(size_t l=0; l<keySize; ++l, ++k) {
            if(k == wordSize) {
                Node *grandChild = current;

                current = new Node {current->key.substr(0, l), *current};
                current->heap.insert(wordIdx);
                grandChild->key = grandChild->key.substr(l);
                current->children[grandChild->key.front() - 'a'] = grandChild;

                // std::cout << "word short" << std::endl;
                goto ENDINSERT;
            }
            else if(word[k] != current->key[l]) {
                Node *origGrandChild = current;
                Node *newGrandChild = new Node{std::string(word.substr(k))};
                newGrandChild->heap.insert(wordIdx);

                current = new Node{current->key.substr(0, l), *current};
                current->heap.insert(wordIdx);
                origGrandChild->key = origGrandChild->key.substr(l);
                current->children[origGrandChild->key.front() - 'a'] = origGrandChild;
                current->children[newGrandChild->key.front() - 'a'] = newGrandChild;

                /*
                std::cout << parent->key << std::endl;
                std::cout << current->key << std::endl;
                std::cout << origGrandChild->key << std::endl;
                std::cout << newGrandChild->key << std::endl;

                std::cout << "split" << std::endl;
                */
                goto ENDINSERT;
            }
        }

        current->heap.insert(wordIdx);
        parent = current;
    } 

    ENDINSERT:
    // displayTrie(theTrie);
    // std::cout << std::endl;
    return;
}

std::vector<std::vector<idx_t>> Trie::getCompletionIdx(std::string_view word, fast_t multiplicity) const {
    std::vector<std::vector<idx_t>> result;
    const size_t wordSize = word.size();
    result.reserve(wordSize + 1);

    Node *current = theTrie;
    result.emplace_back(current->heap.kMost(multiplicity));

    for(size_t idx=0; idx<wordSize;) {
        current = current->getChild(word[idx]);
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
