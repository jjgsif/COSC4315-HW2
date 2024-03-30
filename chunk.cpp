#include <stdlib.h>
#include <stdio.h>

using namespace std;

enum OpCode {
    OP_RETURN,
    OP_CONSTANT
};

typedef double Value;

struct ValueArray{
    int capacity;
    int count;
    Value* values;
};



struct Chunk{
    int count;
    int capacity;
    u_int8_t* code;
    int* lines;
    ValueArray constants;
};

void initChunk(Chunk* chunk){
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = nullptr;
    chunk->lines = nullptr;
    initValueArray(&chunk->constants);
}

static int constantInstruction(const char* name, Chunk* chunk, int offset){
    u_int8_t constant = chunk->code[offset+1];
    printf("%-16s %d '", name, constant);
    printf("%g",chunk->constants.values[constant]);
    printf("'\n'");
    return offset+2;
}


#define GROW_CAPACITY(capacity) \
 ((capacity) < 8 ? 8 : (capacity) * 2)

 #define GROW_ARRAY(type, pointer, oldCount, newCount) \
 (type*)reallocate(pointer, sizeof(type) * (oldCount), \
 sizeof(type) * (newCount))

 #define FREE_ARRAY(type, pointer, oldCount) \
 reallocate(pointer, sizeof(type) * (oldCount), 0)

 void* reallocate(void* pointer, size_t oldSize, size_t newSize){
    if(newSize == 0){
        delete pointer;
        return nullptr;
    }

    void* result = realloc(pointer, newSize);
    return result;
 }

void writeChunk(Chunk* chunk, u_int8_t byte, int line){
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(u_int8_t, chunk->code, oldCapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

void freeChunk(Chunk* chunk){
    FREE_ARRAY(u_int8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    initChunk(chunk);
}


static int simpleInstruction(const char* name, int offset) {
 printf("%s\n", name);
 return offset + 1;
}

int disassembleInstruction(Chunk* chunk, int offset){
    printf("%04d ", offset);
    u_int8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
        default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
 }
}

void disassembleChunk(Chunk* chunk, char* name){
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

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

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}



