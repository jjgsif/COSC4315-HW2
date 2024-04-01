#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "value.cpp"

enum OpCode {
    OP_RETURN,
    OP_CONSTANT,
    OP_NIL, 
    OP_TRUE, 
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL

};

struct Chunk{
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
    ValueArray constants;
};

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);


