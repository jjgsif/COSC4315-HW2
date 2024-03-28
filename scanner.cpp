#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++11-extensions"
#pragma GCC diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

using namespace std;

enum class TokenType{
 // Single-character tokens.
 TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
 TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
 TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
 TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
 // One or two character tokens.
 TOKEN_BANG, TOKEN_BANG_EQUAL,
 TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
 TOKEN_GREATER, TOKEN_GREATER_EQUAL,
 TOKEN_LESS, TOKEN_LESS_EQUAL,
 // Literals.
 TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
 // Keywords.
 TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
 TOKEN_FOR, TOKEN_DEF, TOKEN_IF, TOKEN_IS, TOKEN_OR, TOKEN_ELIF,
 TOKEN_PRINT, TOKEN_RETURN, TOKEN_FINALLY, TOKEN_THIS, TOKEN_NONE,
 TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE, TOKEN_IN,
 TOKEN_ERROR, 
 TOKEN_EOF
};
struct Scanner {
    int line;

    char* start;
    char* current;
};

static Scanner scanner;


struct Token {
    TokenType type;
    char*  start;
    int length;
    int line;
};
// TOKEN TYPES TAKE FROM CRAFTING INTERPRETERS GUIDE


static void initScanner(char* source){
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAtEnd(){
    return *scanner.current == '\0';
}

static Token makeToken(TokenType type){
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current-scanner.start);
    token.line = scanner.line;

    return token;
}

static Token ErrorToken(char* message){
    Token token;
    token.type = TokenType::TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;


    return token;

}

static bool nextToken(char next){
    if(isAtEnd()) return false;
    if(*scanner.current != next) return false;

    scanner.current++;
    return true;
}

static void skipSpace(){
    for(;;){
        switch (*scanner.current){
            case ' ':
            case '\r':
            case '\t':
                scanner.current++;
                break;
            case '\n':
                //cout << scanner.line << endl;
                
                scanner.line++;
                scanner.current++;
                break;
            case '#': 
                while(*scanner.current != '\n' && !isAtEnd()) {scanner.current++;}break;
            default: return;
        }
    }
}

static Token stringToken(){
    while (*scanner.current != '"' && !isAtEnd() && *scanner.current != '\n'){
        scanner.current++;
    }
    char* openString = "Open String";
    if(isAtEnd() || *scanner.current == '\n') return ErrorToken(openString);

    scanner.current++;
    return makeToken(TokenType::TOKEN_STRING);
}
static bool isDigit(char c){
    return c>= '0' && c <='9';
}

static bool isAlpha(char c) {
 return (c >= 'a' && c <= 'z') ||
 (c >= 'A' && c <= 'Z') ||
 c == '_';
}

static TokenType keywordToken(int start, int length, char* restOfKeyword, TokenType type){
    if (scanner.current - scanner.start == start + length &&
    memcmp(scanner.start + start, restOfKeyword, length) == 0) {
    return type;
    }
    return TokenType::TOKEN_IDENTIFIER;
}



static TokenType keywordTokenE(int start, int length){
    if (scanner.current - scanner.start == start + length &&
    memcmp(scanner.start + start, "lif", length) == 0) {
    return TokenType::TOKEN_ELIF;
    }
    if (scanner.current - scanner.start == start + length &&
    memcmp(scanner.start + start, "lse", length) == 0) {
    return TokenType::TOKEN_ELSE;
    }
    


    return TokenType::TOKEN_IDENTIFIER;
}

static Token identifier(){
    while (isAlpha(*scanner.current) || isDigit(*scanner.current)) {scanner.current++;}
    switch (scanner.start[0])
    {
        case 'a': return makeToken(keywordToken(1,2, "an", TokenType::TOKEN_AND));
        case 'n': return makeToken(keywordToken(1,2,"ot", TokenType::TOKEN_BANG));
        case 'd': return makeToken(keywordToken(1,2,"ef", TokenType::TOKEN_DEF));
        case 'f': 
            if(scanner.current - scanner.start > 1){
                switch (scanner.start[1]){
                    case 'a': return makeToken(keywordToken(2,3,"lse", TokenType::TOKEN_FALSE));
                    case 'o': return makeToken(keywordToken(1,2,"or", TokenType::TOKEN_FOR));
                    case 'i': return makeToken(keywordToken(1,6,"inally", TokenType::TOKEN_FINALLY));
                }
            }
            break;
        case 'r': return makeToken(keywordToken(1,5,"eturn", TokenType::TOKEN_RETURN));
        case 'p': return makeToken(keywordToken(1,4,"rint", TokenType::TOKEN_PRINT));
        case 't': return makeToken(keywordToken(1,3,"rue", TokenType::TOKEN_TRUE));

        default: return makeToken(TokenType::TOKEN_IDENTIFIER);
    
    }
    return makeToken(TokenType::TOKEN_IDENTIFIER);
    
}

static Token number(){
    while(isDigit(*scanner.current)){scanner.current++;}

    return makeToken(TokenType::TOKEN_NUMBER);
}



static Token scanToken(){
    skipSpace();


    scanner.start = scanner.current;

    if(isAtEnd()) return makeToken(TokenType::TOKEN_EOF);
    
    char c = *scanner.current;
    scanner.current++;
    if(isAlpha(c)) return identifier();
    if(isDigit(c)) return number();
    switch (c) {

        //SYMBOLS
        case '(': return makeToken(TokenType::TOKEN_LEFT_PAREN);
        case ')': return makeToken(TokenType::TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TokenType::TOKEN_LEFT_BRACE);
        case '}': return makeToken(TokenType::TOKEN_RIGHT_BRACE);
        case ':': return makeToken(TokenType::TOKEN_SEMICOLON);
        case ',': return makeToken(TokenType::TOKEN_COMMA);
        case '.': return makeToken(TokenType::TOKEN_DOT);
        case '-': return makeToken(TokenType::TOKEN_MINUS);
        case '+': return makeToken(TokenType::TOKEN_PLUS);
        case '/': return makeToken(TokenType::TOKEN_SLASH);
        case '*': return makeToken(TokenType::TOKEN_STAR);
        case '!': return makeToken(nextToken('=') ? TokenType::TOKEN_BANG_EQUAL :  TokenType::TOKEN_BANG);
        case '=': return makeToken(nextToken('=') ? TokenType::TOKEN_EQUAL_EQUAL: TokenType:: TOKEN_EQUAL);
        case '<': return makeToken(nextToken('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
        case '>': return makeToken(nextToken('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);

        //LITERALS

        case '"': return stringToken();
    }
    char* uC = "Unexpected Character";
    return ErrorToken(uC);
}

#pragma GCC diagnostic pop