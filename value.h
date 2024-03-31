#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "cmn.cpp"

enum ValueType{
 VAL_BOOL,
 VAL_NIL, 
 VAL_NUMBER,
 VAL_OBJ
};

typedef struct Obj obj;
typedef struct ObjString ObjString;

struct Value{
    ValueType type;
    union {
        bool boolean; 
        double number;
        Obj* obj; 
    } as;
};



#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)


#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)((Value){VAL_OBJ, {.obj = (Obj*)object}})

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

 #define ALLOCATE_OBJ(type, objectType)\
    (type*)allocateObject(sizeof(type), objectType)


struct ValueArray{
    int capacity;
    int count;
    Value* values;
};


void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);


enum ObjType{
    OBJ_STRING
};

struct Obj
{
    ObjType type;
    Obj* next;
};

struct ObjString
{
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};

static inline bool isObjType(Value value, ObjType type){
    return (IS_OBJ(value) && AS_OBJ(value)->type == type);
}

static Obj* allocateObject(size_t size, ObjType type);
void printValue(Value value);
void printObject(Value value);
bool valuesEqual(Value a, Value b);

