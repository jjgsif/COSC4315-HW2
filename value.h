#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "cmn.cpp"

enum ValueType{
 VAL_BOOL,
 VAL_NIL, 
 VAL_NUMBER,
 TRUE_VAL
};

struct Value{
    ValueType type;
    union {
        bool boolean; 
        double number; 
    } as;
};



#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)


#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})


struct ValueArray{
    int capacity;
    int count;
    Value* values;
};


void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);
bool valuesEqual(Value a, Value b);