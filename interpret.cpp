#include <string>
#include "scanner.cpp"
#include "chunk.cpp"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++11-extensions"

using namespace std;

enum class InterpretResult{
    INTERPRET_OK, INTERPRET_COMPILE_ERR, INTERPRET_RUNTIME_ERR
};

struct Parser{
    Token current;
    Token previous;
};


Parser parser;

static void errorAtCurrent(char* message){
    errorAt(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;

    for(;;){
        parser.current = scanToken();
        if(parser.current.type != TokenType::TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}



bool compile(char* sourceCode, Chunk* chunk){
    initScanner(sourceCode);
    
    for (;;) {
        int line = -1;
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf(" | ");
        }
        printf("%d'%.*s'\n", token.type, token.length, token.start); 
        if (token.type == TokenType::TOKEN_EOF) break;
    }

    delete sourceCode;
    return true;
}

InterpretResult interpret( char* sourceCode){
    Chunk chunk;
    initChunk(&chunk);

    if(!compile(sourceCode, &chunk)){
        freeChunk(&chunk);
        return InterpretResult::INTERPRET_COMPILE_ERR;
    }

    vm.chunk = &chunk;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}

#pragma GCC diagnostic pop

