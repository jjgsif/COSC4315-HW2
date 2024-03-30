#include <string>
#include <stdlib.h>
#include "scanner.cpp"
#include "chunk.cpp"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++11-extensions"

using namespace std;



struct Parser{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
};

typedef enum{
    PREC_NONE,
    PREC_ASSIGNMENT,    // =
    PREC_OR,            // or
    PREC_AND,           // and
    PREC_EQUALITY,      // == !=
    PREC_COMPARISON,    // < > <= >=
    PREC_TERM,          // + -
    PREC_FACTOR,        // * /
    PREC_UNARY,         // ! -
    PREC_CALL,          // . ()
    PREC_PRIMARY
}Precedence;

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;

Chunk* compilingChunk;

static Chunk* currentChunk(){
    return compilingChunk;
}

static void errorAt(Token* token, const char* message){
    if(parser.panicMode) return;
    parser.panicMode = true;

    
    cout<<"Error at line: "<<token->line;
    if(token->type == TokenType::TOKEN_EOF){
        cout<<" at end";
    }
    else if(token->type == TokenType::TOKEN_ERROR){
    }
    else{
        cout<<" at "<<token->length<<token->start;
    }
    cout<<message<<endl;
    parser.hadError = true;
}

static void advance() {
    parser.previous = parser.current;

    for(;;){
        parser.current = scanToken();
        if(parser.current.type != TokenType::TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char* message){
    if (parser.current.type == type){
        advance();
        return;
    }
    errorAtCurrent(message);
}

static void emitByte(uint8_t byte){
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2){
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn(){
    emitByte(OP_RETURN);
}

static void emitConstant(Value value){
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void number(bool canAssign){
    double value = strtod(parser.previous.start, NULL);
    emitConstant(value);
}

static void unary(bool canAssign){
    TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch(operatorType){
        case TokenType::TOKEN_MINUS : emitByte(OP_NEGATE); break;
        default:
            return; 
    }

}

ParseRule rules[] = {
    [TokenType::TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TokenType::TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    
};

static void parsePrecedence(Precedence precedence){

}

static ParseRule* getRule(TokenType type){
    return &rules[type];
}

static uint8_t makeConstant(Value value){
    int constant = addConstant(currentChunk(), value);
    if(constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void endCompiler(){
    emitReturn();
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void binary(){
    TokenType operatorType = parser.previous.type;

    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType){
        case TokenType::TOKEN_PLUS:     emitByte(OP_ADD); break;
        case TokenType::TOKEN_MINUS:     emitByte(OP_SUBTRACT); break;
        case TokenType::TOKEN_STAR:     emitByte(OP_MULTIPLY); break;
        case TokenType::TOKEN_SLASH:     emitByte(OP_DIVIDE); break;
        default:
            return;
    }
}

static void expression(){
    parsePrecedence(PREC_ASSIGNMENT);
}

static void grouping(bool canAssign){
    expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

bool compile(char* sourceCode, Chunk* chunk){
    initScanner(sourceCode);
    compilingChunk = chunk;
    
    parser.hadError = false;
    parser.panicMode = false;
    advance();
    expression();
    consume(TokenType::TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}


#pragma GCC diagnostic pop

static void errorAtCurrent(const char* message){
    errorAt(&parser.current, message);
}

static void error(const char* message){
    errorAt(&parser.previous, message);
}

