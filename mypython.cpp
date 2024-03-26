#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

using namespace std;






void checkAssignment(string, map<string, int>&);


int main(int argc, char* argv[]){
    ifstream pyFile(argv[argc-2]);
    ofstream output(argv[argc-1]);
    string line = "";
    map<string, int> variables;
    while(getline(pyFile, line)){
        checkAssignment(line, variables);
    }
}

void checkAssignment(string line, map<string, int>& variables){
    int index = line.find('=');
    if(index != string::npos){
        try {
            string name = line.substr(0, index);
            name.erase(remove(name.begin(), name.end(), char(32)), name.end());
            int value = stoi(line.substr(index + 1));
           variables.insert(pair<string, int>(name, value));
        }catch (exception e){}
    }
}

