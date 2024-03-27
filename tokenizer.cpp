#include <string>
#include <vector>
#include "token.cpp"

using namespace std;



class tokenizer{
    private:
    string fileContents;
    vector<token> tokens;
    int current = 0;
    int start = 0;
    int line = 1;

    public:

    tokenizer(string sourceFile){
        fileContents = sourceFile;
        
    }

    vector<token> scanTokens(){
        while(current < fileContents.length()){
            start = current;
            scanTokens();
        }
        string literal = "";
        tokens.push_back(token(TokenType::END_OF, "", literal , line));
        return tokens;
    }
};