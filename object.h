#pragma once

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

#define IS_OBJ(value) ((value).type == VAL_OBJ)
#define AS_OBJ(value) ((value).as.obj)
#define OBJ_VAL(object)((Value){VAL_OBJ, {.obj = (Obj*)object}})
#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)
#define ALLOCATE_OBJ(type, objectType)\
    (type*)allocateObject(sizeof(type), objectType)

static inline bool isObjType(Value value, ObjType type){
    return (IS_OBJ(value) && AS_OBJ(value)->type == type);
}

static Obj* allocateObject(size_t size, ObjType type);
ObjString* copyString(char* chars, int length);
ObjString* allocateString(char* string, int length);
ObjString* takeString(char* chars, int length);
uint32_t hashString(const char* key, int length);

