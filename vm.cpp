#include "chunk.cpp"
#include <iostream>


enum InterpretResult{
    INTERPRET_OK, INTERPRET_COMPILE_ERR, INTERPRET_RUNTIME_ERR
};

struct VM {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[256];
    Value* stackTop;
};






VM vm;




void push(Value value){
    *vm.stackTop = value; 
    vm.stackTop++;
}

Value pop(){
    vm.stackTop--;
    return *vm.stackTop;
}

static void resetStack(){
    vm.stackTop = vm.stack;
}

void initVM(){
    resetStack();
}



void freeVM(){
    vm.stackTop = vm.stack;
}
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
static InterpretResult run(){
    for(;;){
        uint8_t instruction;
        switch(instruction = READ_BYTE()){
            case OP_CONSTANT: 
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            case OP_RETURN:{
                printf("%g",pop());
                cout << "\n";
                return INTERPRET_OK;
            }
            case OP_NEGATE:
            push(-pop()); break;
        }
    }
}

InterpretResult interpret( Chunk* chunk){
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run(); 
}