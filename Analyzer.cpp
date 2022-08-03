//
// Created by esdras-toc on 2/08/22.
//
using namespace std;
#include "string"
#include "vector"
#include "algorithm"
#include "iostream"
#include "string.h"
#include "fstream"

void ExecuteMkdisk(int size, string unit, char fit, string path);


string ToLower(string data){
    for_each(data.begin(), data.end(), [](char & c){
        c = tolower(c);
    });

    return data;
}

vector<string> Split(string str, char separator){
    int i = 0, state = 0;
    string splitted = "";
    vector<string> results;
    char text[str.length()+1];
    strcpy(text, str.c_str());
    // Mini automata encargado de separar por medio de los espacios en blanco
    // se creo un estado para cadenas de texto entre comillas
    while(i < str.length()){
        if(text[i] == '\n'){
            break;
        }
        if(state == 0){
            if(text[i] == separator){
                i++;
                continue;
            }
            state = 1;
            splitted = splitted + text[i];
        }else if(state == 1){
            if(text[i]==separator){
                results.push_back(splitted);
                state = 0;
                i++;
                splitted = "";
                continue;
            }
            if (text[i] == '"'){
                state = 2;
                i++;
                continue;
            }
            splitted = splitted + text[i];
        }else{
            if (text[i] == '"'){
                state = 1;
                i++;
                continue;
            }
            splitted = splitted + text[i];
        }
        i++;
    }

    if(text[i-1] != ' '){
        results.push_back(splitted);
    }

    return results;
}

/*
 *
 * READ PARAMETERS
 *
 */
int ParameterSize(string str){
    int size;
    try {
        size = stoi(str);
    }catch (...) {
        cout << "$Error: Size parameter must be numeric"<<endl;
        return 0;
    }

    if(size <= 0){
        cout<< "$Error: The size value must be greater than zero"<<endl;
    }else{
        return size;
    }

    return 0;
}

char ParameterFit(string cmd){
    string fit = ToLower(cmd);
    if(fit == "bf"){
        return 'B';
    }else if(fit == "ff"){
        return 'F';
    }else if(fit == "wf"){
        return 'W';
    }

    cout << "$Error: fit type is invalid" << endl;

    return ' ';
}

string ParameterUnit(string cmd){
    string unit = ToLower(cmd);

    if(unit == "m" || unit == "k"){
        return unit;
    }

    cout << "$Error: unit type is invalid"<<endl;

    return "";
}

string ParameterPath(string cmd){
    if(cmd != ""){
        return cmd;
    }

    cout << "$Error: PATH cannot be empty" << endl;
    return "";
}
/*
 *
 * READ COMMANDS
 *
 */
void ReadMkdisk(vector<string> params){
    string path, unit = "m";
    char fit = 'F';
    int size;
    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if (name == "-s-"){
            size = ParameterSize(value);
            if(size == 0){
                return;
            }
        }else if(name == "-f-"){
            fit = ParameterFit(value);
            if(fit == ' '){
                return;
            }
        }else if(name == "-u-"){
            unit = ParameterUnit(value);
            if(unit == ""){
                return;
            }
        }else if(name == "-path-"){
            path = ParameterPath(value);
            if(path == ""){
                return;
            }
        }else{
            cout << "$Error: "+name+" is not a valid parameter"<<endl;
            return;
        }
    }


    if(path == ""){
        cout << "$Error: PATH is a mandatory parameter" << endl;
        return;
    }

    if(size == 0){
        cout << "$Error: PATH is a mandatory parameter" << endl;
        return;
    }

    ExecuteMkdisk(size, unit, fit, path);
}

void Read(string str){
    vector<string> command;
    command = Split(str, ' ');
    string cmd = ToLower(command.at(0));
    command.erase(command.begin());

    if(cmd == "mkdisk"){
        ReadMkdisk(command);
    }else{
        cout << cmd << " command not recognized" << endl;
    }
}