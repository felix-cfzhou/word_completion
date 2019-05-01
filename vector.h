#ifndef VECTOR
#define VECTOR

#include <cstdlib>
#include <utility>

#include "types.h"


template<typename T> struct Vector {
    T *theVector;
    fast_t theSize;
    fast_t theCap;

    Vector(fast_t initialCap):
        theVector{static_cast<T*>(malloc(initialCap*sizeof(T)))},
        theSize{0},
        theCap{initialCap}
    {}

    fast_t size() const {return theSize;}
    T &operator[](fast_t idx) {return theVector[idx];}
    const T &operator[](fast_t idx) const {return theVector[idx];}
    template<typename ...Args> void emplace_back(Args &&...args) {
        increaseCap();
        theVector[theSize++] = T(std::forward<Args>(args)...);
    }
    T &back() {return theVector[theSize-1];}
    bool empty() const {return !theSize;}

    void increaseCap() {
        if(theSize == theCap) {
            theVector = static_cast<T*>(realloc(theVector, 3*theCap*sizeof(T)));
            theCap *= 3;
        }
    }

    ~Vector() {free(theVector);}
};

#endif
