#include <string>
#include "scanner.cpp"

using namespace std;

enum class InterpretResult{
    INTERPRET_OK, INTERPRET_COMPILE_ERR, INTERPRET_RUNTIME_ERR
};



InterpretResult interpret(const string sourceCode){
    compile(sourceCode);
    return InterpretResult::INTERPRET_OK;
}

void compile(const string sourceCode){
    scanner(sourceCode);
}