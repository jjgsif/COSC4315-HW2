#include "assignment.h"

void initHashT(hashT *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = nullptr;
}

void freeHashT(hashT *table)
{
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initHashT(table);
}

bool tableSet(hashT *table, ObjString *key, Value value)
{
    Entry *entry = findEntry(table->entries, table->capacity, key);

    bool isNewKey = entry->key == nullptr;
    if (isNewKey && IS_NIL(entry->value))
        table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

Entry *findEntry(Entry *entries, int capacity, ObjString *key)
{
    uint32_t index = key->hash % capacity;
    Entry *tombstone = NULL;
    for (;;)
    {
        Entry *entry = &entries[index];
        if (entry->key == NULL){
            if (IS_NIL(entry->value)){
                return tombstone != NULL ? tombstone : entry;
            }
            else{
                if(tombstone == NULL)tombstone = entry;
            }
        }
        else if (entry->key == key){
            return entry;
        }
            index = (index + 1) % capacity;
    }
}

void adjustCapacity(hashT *table, int capacity)
{
    Entry *entries = ALLOCATE(Entry, capacity);
    table->count = 0;
    for (int i = 0; i < capacity; i++)
    {
        Entry *entry = &table->entries[i];
        if (entry->key == nullptr)
            continue;

        Entry *dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

void hashTaddAll(hashT *origin, hashT *dest)
{
    for (int i = 0; i < origin->capacity; i++)
    {
        Entry *entry = &origin->entries[i];
        if (entry->key != nullptr)
        {
            tableSet(dest, entry->key, entry->value);
        }
    }
}

ObjString* tableFindString(hashT* table, const char* chars, int length, uint32_t hash){
    if(table->count == 0 )return NULL;

    uint32_t index = hash % table->capacity;

    for(;;){
        Entry* entry = &table->entries[index];
        if(entry->key == NULL){

            if(IS_NIL(entry->value)) return NULL;
        }
        else if(entry->key->length == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0){

            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}

bool tableGet(hashT *table, ObjString *key, Value *value)
{
    if (table->count == 0)
        return false;
    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;
    *value = entry->value;
    return true;
}

bool tableDelete(hashT *table, ObjString *key)
{
    if(table->count == 0) return false;

    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;
    entry->key = NULL;
    entry->value = BOOL_VAL(true);
    return true;
}
