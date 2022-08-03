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
            }
            splitted = splitted + text[i];
        }else{
            if (text[i] == '"'){
                state = 1;
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

void Read(string str){
    vector<string> command;
    command = Split(str, ' ');
    string cmd = ToLower(command.at(0));
    command.erase(command.begin());

    /*int i = 0;
    while(i < command.size()){
        cout << command.at(i)<<'*'<<endl;
        i++;
    }*/

    if(cmd == "mkdisk"){
        cout << "comando mkdisk" << endl;
    }else{
        cout << cmd << " command not recognized" << endl;
    }
}