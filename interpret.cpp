#include <string>
#include "scanner.cpp"


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

        errorAtCurrent(parser.current.start);
    }
}



bool compile(char* sourceCode, Chunk* chunk){
    initScanner(sourceCode);
    
    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
}
#pragma GCC diagnostic pop

