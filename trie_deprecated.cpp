#include <iostream>

#include "trie.h"


void Trie::access(std::string_view word, idx_t wordIdx) {
    // std::cout << "accessing: " << word << std::endl;
    Node *current = theTrie;
    current->heap.fixUp(wordIdx);

    const size_t wordSize = word.size();
    for(size_t k=0; k<wordSize;) {
        // std::cout << current->key << ':' << std::endl;
        // for(auto child : current->children) if(child) std::cout << child->key << std::endl;
        current = (*current)[word[k]];
        current->heap.fixUp(wordIdx);

        k += current->key.size();
    } 
}

void Trie::insert(std::string_view word, idx_t wordIdx) {
    // std::cout << "inserting: " << word << std::endl;

    Node *parent = theTrie;
    parent->heap.insert(wordIdx);

    const size_t wordSize = word.size();
    for(size_t k=0; k<wordSize;) {
        Node *&current = (*parent)[word[k]];
        if(!current) {
            current = new Node{std::string(word.substr(k)), wordIdx};
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

                current = new Node {current->key.substr(0, l), wordIdx, *current};
                current->heap.insert(wordIdx);
                grandChild->key.erase(0, l);
                (*current)[grandChild->key.front()] = grandChild;

                // std::cout << "word short" << std::endl;
                goto ENDINSERT;
            }
            else if(word[k] != current->key[l]) {
                Node *origGrandChild = current;
                Node *newGrandChild = new Node{std::string(word.substr(k)), wordIdx};
                newGrandChild->heap.insert(wordIdx);

                current = new Node{current->key.substr(0, l), -1, *current};
                current->heap.insert(wordIdx);
                origGrandChild->key.erase(0, l);
                (*current)[origGrandChild->key.front()] = origGrandChild;
                (*current)[newGrandChild->key.front()] = newGrandChild;

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
        if(k == wordSize) /*assert(current->idx == -1),*/ current->idx = wordIdx;
    } 

    ENDINSERT:
    // displayTrie(theTrie);
    // std::cout << std::endl;
    return;
}

void Trie::displayTrie(Trie::Node *trie, size_t indent=0) const {
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
