#ifndef FIXED_SIZE_ALLOCATOR
#define FIXED_SIZE_ALLOCATOR

#include <cstdlib>

#include "types.h"


template<typename T, fast_t N=1024> struct FixedSizeAllocator {
    // we allocate blocks of space for Trie Nodes for better cache performance
    constexpr static size_t defaultSize = 64;

    T** theBlocks;
    fast_t sizeBlocks;
    fast_t capBlocks;
    fast_t nextSlot;

    FixedSizeAllocator():
        theBlocks{static_cast<T**>(malloc(defaultSize*sizeof(T*)))},
        sizeBlocks{1},
        capBlocks{defaultSize},
        nextSlot{0}
    {   
        theBlocks[0] = static_cast<T*>(malloc(N*sizeof(T)));
    }   

    T *allocate() noexcept {
        // give our one slot
        if(nextSlot == N) {
            if(sizeBlocks == capBlocks) {
                theBlocks = static_cast<T**>(realloc(theBlocks, 3*capBlocks*sizeof(T*)));
                capBlocks *= 3;
            }   
            theBlocks[sizeBlocks++] = static_cast<T*>(malloc(N*sizeof(T)));
            nextSlot = 0;
        }   

        return &theBlocks[sizeBlocks-1][nextSlot++];
    }

    // We do not need deallocate as we never delete a Trie Node

    [[ using gnu : hot ]]
    ~FixedSizeAllocator() {
        for (fast_t k=0; k<sizeBlocks-1; ++k) {
            for(fast_t l=0; l<N; ++l) {
                theBlocks[k][l].~T();
            }
            free(theBlocks[k]);
        }

        for(fast_t k=0; k<nextSlot; ++k) {
            theBlocks[sizeBlocks-1][k].~T();
        }
        free(theBlocks[sizeBlocks-1]);
    
        free(theBlocks);
    }
};

#endif
