#include "interpret.h"

using namespace std;
bool ifDone = false;

ParseRule rules[] = {
    [TokenType::TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TokenType::TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_LEFT_BRACE] = {block, NULL, PREC_NONE},
    [TokenType::TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_COMMA] = {NULL, binary, PREC_TERM},
    [TokenType::TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TokenType::TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TokenType::TOKEN_ENDLINE] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TokenType::TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_COLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_TAB] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TokenType::TOKEN_BANG_EQUAL] = {NULL, binary, PREC_NONE},
    [TokenType::TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TokenType::TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TokenType::TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TokenType::TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TokenType::TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TokenType::TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TokenType::TOKEN_STRING] = {stringI, NULL, PREC_NONE},
    [TokenType::TOKEN_NUMBER] = {numberI, NULL, PREC_NONE},
    [TokenType::TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TokenType::TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_DEF] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_IS] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_NONE] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TokenType::TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_IN] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TokenType::TOKEN_EOF] = {NULL, NULL, PREC_NONE}};

Parser parser;

Chunk *compilingChunk;

static void namedVariable(Token name)
{
    uint8_t arg = identifierConstant(&name);
    emitBytes(OP_GET_GLOBAL, arg);
}

static void variable()
{
    namedVariable(parser.previous);
}

static Chunk *currentChunk()
{
    return compilingChunk;
}

static void errorAt(Token *token, const char *message)
{
    if (parser.panicMode)
        return;
    parser.panicMode = true;

    cout << "Error at line: " << token->line;
    if (token->type == TokenType::TOKEN_EOF)
    {
        cout << " at end";
    }
    else if (token->type == TokenType::TOKEN_ERROR)
    {
    }
    else
    {
        cout << " at " << token->length << " " << token->start;
    }
    cout << message << endl;
    parser.hadError = true;
}

static void advance()
{
    parser.previous = parser.current;
    for (;;)
    {
        parser.current = scanToken();
        
        if (parser.current.type != TokenType::TOKEN_ERROR)
            break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }
    errorAtCurrent(message);
}

static void emitByte(uint8_t byte)
{
    writeChunk(currentChunk(), byte, parser.previous.line);
    
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn()
{
    emitByte(OP_RETURN);
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static int emitJump(uint8_t instruction) {
 emitByte(instruction);
 emitByte(0xff);
 emitByte(0xff);
 return currentChunk()->count - 2;
}

static void patchJump(int offset) {
 int jump = currentChunk()->count - offset - 2;
 //cout << jump << endl;
 if(ifDone){
    if(jump == 5){
        jump+=4;
    }
 }
 if (jump > UINT16_MAX) {
 error("Too much code to jump over.");
 }
 currentChunk()->code[offset] = (jump >> 8) & 0xff; 
 currentChunk()->code[offset + 1] = jump & 0xff;
 ifDone = true;
}


static void literal()
{
    switch (parser.previous.type)
    {
    case TokenType::TOKEN_FALSE:
        emitByte(OP_FALSE);
        break;
    case TokenType::TOKEN_TRUE:
        emitByte(OP_TRUE);
        break;
    default:
        return;
    }
}

static void stringI()
{
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser
                                                                       .previous.length -
                                                                   2)));
}

void binary()
{
    TokenType operatorType = parser.previous.type;

    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType)
    {
    case TokenType::TOKEN_BANG_EQUAL:
        emitBytes(OP_EQUAL, OP_NOT);
        break;
    case TokenType::TOKEN_EQUAL_EQUAL:
        emitByte(OP_EQUAL);
        break;
    case TokenType::TOKEN_GREATER:
        emitByte(OP_GREATER);
        break;
    case TokenType::TOKEN_GREATER_EQUAL:
        emitBytes(OP_LESS, OP_NOT);
        break;
    case TokenType::TOKEN_LESS:
        emitByte(OP_LESS);
        break;
    case TokenType::TOKEN_LESS_EQUAL:
        emitBytes(OP_GREATER, OP_NOT);
        break;
    case TokenType::TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TokenType::TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TokenType::TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TokenType::TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    case TOKEN_COMMA:
        emitByte(OP_CONCAT);
        break;
    default:
        return;
    }
}
void grouping()
{
    expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void numberI()
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

void unary()
{
    TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType)
    {
    case TokenType::TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;
    case TokenType::TOKEN_BANG:
        emitByte(OP_NOT);
        break;
    default:
        return;
    }
}
typedef void (*ParseFn)();

static void parsePrecedence(Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL)
    {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infixR = getRule(parser.previous.type)->infix;
        infixR();
    }
}

static ParseRule *getRule(TokenType type)
{
    return &rules[(int)type];
}

static uint8_t makeConstant(Value value)
{
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void printStatement()
{
    expression();
    consume(TOKEN_ENDLINE, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

static bool check(TokenType type)
{
    return parser.current.type == type;
}

static void endCompiler()
{
    emitReturn();
}

static bool match(TokenType type)
{
    if (!check(type))
        return false;
    advance();
    return true;
}

static void expressionStatement()
{
    expression();
    consume(TOKEN_ENDLINE, "Expect ';' after expression.");
    emitByte(OP_POP);
}
static void block(){
    declaration();
}



static void statement()
{
    if(match(TOKEN_TAB)){
        //cout << "TAB" << endl;
        block();
    }
    
    else if (match(TOKEN_IF)){
        ifStatement();
    } else if (match(TOKEN_PRINT))
    {
        printStatement();
    }
    else if (!match(TOKEN_EOF) && !match(TOKEN_ENDLINE))
    {
        printStatement();
    }
    
}

static void defineVariable(uint8_t global)
{
    emitBytes(OP_DEFINE_GLOBAL, global);
}

static uint8_t identifierConstant(Token *name)
{
    return makeConstant(OBJ_VAL(copyString(name->start,
                                           name->length)));
}

static void expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}



static void varDeclaration()
{
    uint8_t global = identifierConstant(&parser.previous);

    if (check(TOKEN_EQUAL))
    {
        consume(TOKEN_EQUAL,"");
        expression();
    }
    else{
        emitByte(OP_NIL);
    }
    consume(TOKEN_ENDLINE, "Error");
    defineVariable(global);
}

static void declaration()
{
    if (match(TOKEN_IDENTIFIER))
    {
        varDeclaration();
    }
    else
    {
        statement();
    }
}

static void ifStatement(){
    
    expression();
    int thenJump = emitJump(OP_JUMP_IF_FALSE); 
    emitByte(OP_POP);
    declaration();
    int elseJump = emitJump(OP_JUMP);
    emitByte(OP_POP);
    patchJump(thenJump);
    if (match(TOKEN_ELSE)) {declaration();}
    patchJump(elseJump);
}

bool compile(char *sourceCode, Chunk *chunk)
{
    initScanner(sourceCode);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;
    advance();
    while (!match(TOKEN_EOF))
    {
        declaration();
    }
    endCompiler();
    return !parser.hadError;
}

static void errorAtCurrent(const char *message)
{
    errorAt(&parser.current, message);
}

static void error(const char *message)
{
    errorAt(&parser.previous, message);
}
