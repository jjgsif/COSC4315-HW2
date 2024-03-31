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
    if (isNewKey)
        table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

Entry *findEntry(Entry *entries, int capacity, ObjString *key)
{
    uint32_t index = key->hash % capacity;
    for (;;)
    {
        Entry *entry = &entries[index];
        if (entry->key == key || entry->key == NULL)
        {
            return entry;
        }
        index = (index + 1) % capacity;
    }
}

void adjustCapacity(hashT *table, int capacity)
{
    Entry *entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++)
    {
        Entry *entry = &table->entries[i];
        if (entry->key == nullptr)
            continue;

        Entry *dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
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
