#include <string>
#include "scanner.cpp"
#include "vm.cpp"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++11-extensions"

using namespace std;



struct Parser{
    Token current;
    Token previous;
};


Parser parser;



static void advance() {
    parser.previous = parser.current;

    for(;;){
        parser.current = scanToken();
        if(parser.current.type != TokenType::TOKEN_ERROR) break;
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
#pragma GCC diagnostic pop

