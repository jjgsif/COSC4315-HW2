#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;






void runFile(char*);
void run(string);
// void runFile(char*, char*);
// void runPrompt();


int main(int argc, char* argv[]){
    
    switch(argc){
        case 2: runFile(argv[1]); break;
    }
}

void runFile(char* inputFile){
    ifstream input(inputFile, ios::binary | ios::ate);

    if(!input.is_open()){
        cerr << inputFile << " not found. Exiting" << endl;
        return;
    }

    streamsize size = input.tellg();
    input.seekg(0, ios_base::beg);

    vector<char> buffer(size);

    if(!input.read(buffer.data(), size)){
        cerr << "Error reading " << inputFile <<" exiting..." << endl;
        return;
    }

    string joinedVector = "";

    for(int i = 0; i < buffer.size(); i++){
        joinedVector+= buffer[i];
    }
    
    run(joinedVector);

    return;
    
}

void run(string fileContents){
    
}
