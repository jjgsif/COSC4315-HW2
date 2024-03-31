#include <iostream>

#include "chunk.h"

static int constantInstruction(const char* name, Chunk* chunk, int offset);
static int simpleInstruction(const char* name, int offset);
int disassembleInstruction(Chunk* chunk, int offset);
void disassembleChunk(Chunk* chunk, char* name);