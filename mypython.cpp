#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "vm.cpp"


using namespace std;





void runFile(char*);
void run(string);
void console();


int main(int argc, char* argv[]){
    initVM();

    switch (argc){
        case 1: console(); break;
        case 2: runFile(argv[1]); break;
        default: cout << "Usage: ./mypython [filePath]" << endl; break;
    }

    freeVM();

    return 0;
}

void console(){
    char line[1024];

    for(;;){
        cout << "> ";

        if(!fgets(line, sizeof(line), stdin)) {
            cout << "\n";
            break;
        }

        interpret(line);
    }
}

void runFile(char* input){
    ifstream source(input, ios::binary | ios::ate);
    
    if(!source.is_open()){
        cout << input << " not found. Exiting" << endl;
        exit(6);
    }

    streamsize size = source.tellg();
    source.seekg(0, ios_base::beg);

    vector<char> buffer(size+1);

     if(!source.read(buffer.data(), size)){
        cerr << "Error reading " << input <<" exiting..." << endl;
        exit(5);
    } 
    char* joinedVector = new char[size+1];
    buffer[size] = '\0';

    for(int i = 0; i < buffer.size(); i++){
        joinedVector[i] = buffer[i];
    }

    source.close();
    
    InterpretResult result = interpret(joinedVector);

    buffer.erase(buffer.begin(), buffer.end());

    if(result == InterpretResult::INTERPRET_COMPILE_ERR) exit(3);
    if(result == InterpretResult::INTERPRET_RUNTIME_ERR) exit(4);

}





