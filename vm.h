#include "interpret.cpp"
#include <iostream>
#include <map>

enum InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERR,
    INTERPRET_RUNTIME_ERR,
};

struct VM {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[256];
    Value* stackTop;

    Obj* objects;
    map<string, Value> variables;
};







