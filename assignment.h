#include "value.cpp"

struct Entry{
    ObjString *key;
    Value value;
};

struct hashT{
    int count;
    int capacity;
    Entry* entries;
};

#define TABLE_MAX_LOAD 0.75

void initHashT(hashT* table);
void freeHashT(hashT* table);
bool tableSet(hashT* table, ObjString* key, Value value);
Entry* findEntry(Entry* entries, int capacity, ObjString* key);
void adjustCapacity(hashT* table, int capacity);
void tableAddAll(hashT* from, hashT* to);
bool tableGet(hashT* , ObjString* , Value* );
bool tableDelete(hashT*, ObjString*);