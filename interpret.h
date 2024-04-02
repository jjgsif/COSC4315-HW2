#include <string>
#include <stdlib.h>
#include <iostream>
#include "chunk.cpp"
#include "scanner.cpp"

using namespace std;

struct Parser
{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;

};

struct compiler{
    int scopeDepth;
};

enum Precedence
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
};



typedef void (*ParseFn)();

typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

static void expression();
static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);
static Chunk *currentChunk();
static void errorAt(Token *token, const char *message);
static void advance();
static void consume(TokenType type, const char *message);
static void emitByte(uint8_t byte);
static void emitBytes(uint8_t byte1, uint8_t byte2);
static void emitReturn();
static void emitConstant(Value value);
static void literal();
bool compile(char* sourceCode, Chunk* chunk);
void binary();
void grouping();
static void numberI();
void unary();
static void endCompiler();
static void expression();
static void error(const char* message);
static uint8_t makeConstant(Value value);
static void errorAtCurrent(const char* message);
static void stringI();
static void statement();
static void declaration();
static void variable();
static void namedVariable(Token name);
static uint8_t identifierConstant(Token* name) ;
static void block();
static void ifStatement();
static void varDeclaration();




