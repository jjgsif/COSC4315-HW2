<<<<<<< Updated upstream
#include "value.h" //object.h
#include "vm.h" // vm.h
#include "assignment.h" //table.h
=======
#include "value.h"

#include <cstring>
>>>>>>> Stashed changes

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
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if(interned != nullptr) return interned;
    char* cString = ALLOCATE(char, length+1);
    memcpy(cString, chars, length);
    cString[length] = '\0';


    return allocateString(cString, length);
}

uint32_t hashString(const char* key, int length) {
 uint32_t hash = 2166136261u;
 for (int i = 0; i < length; i++) {
 hash ^= key[i];
 hash *= 16777619;
 }
 return hash;
}

ObjString* allocateString(char* string, int length){
    ObjString* stringOut = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    stringOut->length = length;
    stringOut->chars = string;

    tableSet(&vm.strings, stringOut, NIL_VAL);

    return stringOut;
}

ObjString* takeString(char* chars, int length){
    uint32_t hash = hashString(chars, length);
<<<<<<< Updated upstream
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if(interned !=nullptr){
        FREE_ARRAY(char, chars, length +1);
        return interned;
    }   
    return allocateString(chars, length, hash);
=======
    return allocateString(chars, length);
>>>>>>> Stashed changes
}

static Obj* allocateObject(size_t size, ObjType type) {
 Obj* object = (Obj*)reallocate(nullptr, 0, size);
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
        case OBJ_STRING: cout << "\'"+string(AS_CSTRING(value)) + "\'"; break;
    }
}

bool valuesEqual(Value a, Value b) {
 if (a.type != b.type) return false;
 switch (a.type) {
 case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
 case VAL_NIL: return true;
 case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
 case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
 default:
 return false; // Unreachable.
 }
}





