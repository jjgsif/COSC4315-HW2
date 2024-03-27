#include <string>

using namespace std;

enum class TokenType{
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_THAN, LESS, LESS_EQUAL,

    STRING, NUMERIC,

    AND, DEF, IF, ELSE, FOR, IN, OR, WHILE, VAR, TRUE, FALSE, RETURN, 

    END_OF


};


class token{
    private:
    TokenType type;
    string subString;
    string literal;
    int line;

    string getTokenTypeName(TokenType type){
        switch(type){
            //Make Types for the above
            default: return "inProgress";
        }
    }

    public:
    token(TokenType _type, string _substring, string _literal, int _line){
        type = _type;
        subString = _substring;
        line = _line;
        literal = _literal;
    }
    string toString(){
        return getTokenTypeName(type) + " " + subString + literal;
    }
};