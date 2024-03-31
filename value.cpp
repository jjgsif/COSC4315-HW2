#include "value.h"


using namespace std;

Value value;
ObjString* copyString(char* chars, int length);
ObjString* allocateString(char* string, int length);
ObjString* takeString(char* chars, int length);
uint32_t hashString(const char* key, int length);

void initValueArray(ValueArray* array){
    array->values = nullptr;
    array->capacity = 0;
    array->count = 0;
}

void writeValueArray(ValueArray* array, Value value){
    if(array->capacity < array->count + 1){
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array){
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

ObjString* copyString(char* chars, int length){
    uint32_t hash = hashString(chars, length);
    char* cString = ALLOCATE(char, length+1);
    memcpy(cString, chars, length);
    cString[length] = '\0';


    return allocateString(cString, length, hash);
}

uint32_t hashString(const char* key, int length) {
 uint32_t hash = 2166136261u;
 for (int i = 0; i < length; i++) {
 hash ^= key[i];
 hash *= 16777619;
 }
 return hash;
}

ObjString* allocateString(char* string, int length, uint32_t hash){
    ObjString* stringOut = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    stringOut->length = length;
    stringOut->chars = string;
    stringOut->hash = hash;

    return stringOut;
}

ObjString* takeString(char* chars, int length){
    uint32_t hash = hashString(chars, length);
    return allocateString(chars, length, hash);
}

static Obj* allocateObject(size_t size, ObjType type) {
 Obj* object = (Obj*)reallocate(NULL, 0, size);
 object->type = type;
 return object;
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL: printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL: printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ: printObject(value); break;
 }
}

void printObject(Value value){
    switch(OBJ_TYPE(value)){
        case OBJ_STRING: cout << AS_CSTRING(value) << endl; break;
    }
}

bool valuesEqual(Value a, Value b) {
 if (a.type != b.type) return false;
 switch (a.type) {
 case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
 case VAL_NIL: return true;
 case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
 case VAL_OBJ:{
    ObjString* aString = AS_STRING(a);
    ObjString* bString = AS_STRING(b);
    return (aString->length == bString->length && memcmp(aString->chars, bString->chars,aString->length)==0);
 }
 default:
 return false; // Unreachable.
 }
}





