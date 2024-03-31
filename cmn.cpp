#include "cmn.h"


void* reallocate(void* pointer, size_t oldSize, size_t newSize){
    if(newSize == 0){
        delete pointer;
        return nullptr;
    }

    void* result = realloc(pointer, newSize);
    return result;
}