#include <string>
#include "scanner.cpp"

using namespace std;

enum class InterpretResult{
    INTERPRET_OK, INTERPRET_COMPILE_ERR, INTERPRET_RUNTIME_ERR
};

void compile(char* sourceCode){
    initScanner(sourceCode);
    int line = -1;
    for (;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf(" | ");
        }
        printf("'%.*s'\n", token.length, token.start); 
        if (token.type == TokenType::TOKEN_EOF) break;
    }

    delete sourceCode;
    return;
}

InterpretResult interpret( char* sourceCode){
    compile(sourceCode);
    return InterpretResult::INTERPRET_OK;
}



