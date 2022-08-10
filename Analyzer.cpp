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
#include "Structures.h"

void ExecuteMkdisk(int size, string unit, char fit, string path);
void ExecuteRmdisk(string path);
void ExecuteFdiskNewPartition(int size, string unit, string path, string type, char fit, string name);
void ExecuteFdiskAddPartition(int add, string unit, string path, string name);
void ExecuteFdiskDeletePartition(string path, string nameString);
void ExecuteMount(string p, string nameString, vector<MountedPartition> *partitions);
void ExecuteUnmount(string id, vector<MountedPartition> *partitions);
void ExecuteExec(string path);

vector<MountedPartition> mountedPartitions;

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

string ParameterUnitWB(string cmd){
    string unit = ToLower(cmd);

    if(unit == "m" || unit == "k" || unit == "b"){
        return unit;
    }

    cout << "$Error: unit type is invalid"<<endl;

    return "";
}

string ParameterType(string cmd){
    string unit = ToLower(cmd);

    if(unit == "p" || unit == "e" || unit == "l"){
        return unit;
    }

    cout << "$Error: type of partition is invalid"<<endl;

    return "";
}

bool ParameterDelete(string cmd){
    string val = ToLower(cmd);

    if(val == "full"){
        return true;
    }

    cout << "$Error: "<<cmd<<" is not valid"<<endl;
    return false;
}

string ParameterName(string cmd){
    if(cmd == ""){
        cout << "$Error: name cannot be empty"<<endl;
        return "";
    }

    return cmd;
}

int ParameterAdd(string str){
    int size;
    try {
        size = stoi(str);
    }catch (...) {
        cout << "$Error: Size parameter must be numeric"<<endl;
        return 0;
    }

    return size;
}

string ParameterId(string str){
    if(str != ""){
        return str;
    }

    cout << "$Error: Id parameter cannot be empty"<<endl;
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
    int size = 0;
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
        cout << "$Error: SIZE is a mandatory parameter" << endl;
        return;
    }

    ExecuteMkdisk(size, unit, fit, path);
}

void ReadRmdisk(vector<string> params){
    string path = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-path-"){
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

    ExecuteRmdisk(path);
}

void ReadFdisk(vector<string> params){
    string path = "", unit = "k", partitionName = "", type = "p";
    char fit = 'W';
    int size = 0, add=0;
    bool isAdd = false, isDelete = false;
    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if (name == "-s-"){
            size = ParameterSize(value);
            if(size == 0){
                return;
            }
        }else if(name == "-u-"){
            unit = ParameterUnitWB(value);
            if(unit == ""){
                return;
            }
        }else if(name == "-path-"){
            path = ParameterPath(value);
            if(path == ""){
                return;
            }
        }else if(name == "-t-"){
            type = ParameterType(value);
            if(type == ""){
                return;
            }
        }else if(name == "-f-"){
            fit = ParameterFit(value);
            if(fit == ' '){
                return;
            }
        }else if(name == "-delete-"){
            isDelete = ParameterDelete(value);
            if(!isDelete){
                return;
            }
        }else if(name == "-name-"){
            partitionName = ParameterName(value);
            if(partitionName == ""){
                return;
            }
        }else if(name == "-add-"){
            add = ParameterAdd(value);
            isAdd = true;
        }else{
            cout << "$Error: "+name+" is not a valid parameter"<<endl;
            return;
        }
    }

    if(path == ""){
        cout << "$Error: PATH is a mandatory parameter" << endl;
        return;
    }

    if(partitionName == ""){
        cout << "$Error: NAME is a mandatory parameter" << endl;
        return;
    }

    if(isAdd){
        ExecuteFdiskAddPartition(add, unit, path, partitionName);
    }else if(isDelete){
        ExecuteFdiskDeletePartition(path, partitionName);
    }else{
        if(size == 0){
            cout << "$Error: SIZE is a mandatory parameter" << endl;
            return;
        }

        ExecuteFdiskNewPartition(size, unit, path, type, fit, partitionName);
    }
}

void ReadMount(vector<string> params){
    string path = "", partitionName = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-path-"){
            path = ParameterPath(value);
            if(path == ""){
                return;
            }
        }else if(name == "-name-"){
            partitionName = ParameterName(value);
            if(partitionName == ""){
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

    if(partitionName == ""){
        cout << "$Error: NAME is a mandatory parameter" << endl;
        return;
    }

    ExecuteMount(path, partitionName, &mountedPartitions);
}

void ReadUnmount(vector<string> params){
    string id = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-id-"){
            id = ParameterId(value);
            if(id == ""){
                return;
            }
        }else{
            cout << "$Error: "+name+" is not a valid parameter"<<endl;
            return;
        }
    }

    if(id == ""){
        cout << "$Error: ID is a mandatory parameter" << endl;
        return;
    }

    ExecuteUnmount(id, &mountedPartitions);
}

void ReadExec(vector<string> params){
    string path = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-path-"){
            path = ParameterId(value);
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

    ExecuteExec(path);
}

void ReadReport(vector<string> params){

}

void Read(string str){
    vector<string> command;
    command = Split(str, ' ');
    string cmd = ToLower(command.at(0));
    command.erase(command.begin());

    if(cmd == "mkdisk"){
        ReadMkdisk(command);
    }else if(cmd == "rmdisk"){
        ReadRmdisk(command);
    }else if(cmd == "fdisk"){
        ReadFdisk(command);
    }else if(cmd == "rep"){
        ReadReport(command);
    }else if(cmd == "mount"){
        ReadMount(command);
    }else if(cmd == "unmount"){
        ReadUnmount(command);
    }else if(cmd == "exec"){
        ReadExec(command);
    }else{
        cout << cmd << " command not recognized" << endl;
    }
}