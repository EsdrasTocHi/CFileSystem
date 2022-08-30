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
void ExecuteMkfs(string id, int fs, vector<MountedPartition> *partitions);
void ExecuteReport(string id, string name, string path, vector<MountedPartition> *partitions, string ruta);
void ExecuteLogin(string usr, string passw, string id, vector<MountedPartition> *partitions, Sesion *currentUser, bool *activeSession);
void ExecuteLogout(Sesion *currentUser, bool *activeSession);
void ExecuteMkGrp(string name, Sesion *currentUser, bool *activeSession);
void ExecuteMkusr(string name, string pass, string group, Sesion *currentUser, bool *activeSession);
void ExecuteRmgrp(string name, Sesion *currentUser, bool *activeSession);
void ExecuteRmusr(string name, Sesion *currentUser, bool *activeSession);

vector<MountedPartition> mountedPartitions;
Sesion currentUser;
bool activeUser = false;

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

bool ParameterTypeFull(string cmd){
    string val = ToLower(cmd);

    if(val == "full"){
        return true;
    }

    cout << "$Error: "<<cmd<<" is not valid"<<endl;
    return false;
}

int ParameterFs(string str){
    str = ToLower(str);

    if(str == "2fs"){
        return 2;
    }

    if(str == "3fs"){
        return 3;
    }

    cout << "$Error: " << str << " is not valid" << endl;
    return 0;
}

string ParameterCredential(string str){
    if(str == ""){
        cout << "$Error: credentials must not be empty" << endl;
        return "";
    }

    return str;
}

string ParameterGrp(string str){
    if(str == ""){
        cout << "$Error: GRP must not be empty" << endl;
        return "";
    }

    return str;
}

int ParameterUGO(string str){
    int size;
    try {
        size = stoi(str);
    }catch (...) {
        cout << "$Error: UGO parameter must be numeric"<<endl;
        return -1;
    }

    if(size <= 0){
        cout<< "$Error: The UGO value must be greater than zero"<<endl;
    }else if(size > 777){
        cout<< "$Error: The UGO value must be less than 777"<<endl;
    }else{
        return size;
    }

    return -1;
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

void ReadMkfs(vector<string> params){
    string id = "";
    bool full = true;
    int fs = 2;

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-id-"){
            id = ParameterPath(value);
            if(id == ""){
                return;
            }
        }else if(name == "-type-"){
            full = ParameterTypeFull(value);
            if(!full){
                return;
            }
        }else if(name == "-fs-"){
            fs = ParameterFs(value);
            if(fs == 0){
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

    ExecuteMkfs(id, fs, &mountedPartitions);
}

void ReadLogin(vector<string> params){
    string id = "", usr = "", passw = "";
    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-id-"){
            id = ParameterPath(value);
            if(id == ""){
                return;
            }
        }else if(name == "-usr-"){
            usr = ParameterCredential(value);
            if(usr == ""){
                return;
            }
        }else if(name == "-pass-"){
            passw = ParameterCredential(value);
            if(passw == ""){
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
    if(usr == ""){
        cout << "$Error: USR is a mandatory parameter" << endl;
        return;
    }
    if(passw == ""){
        cout << "$Error: PASS is a mandatory parameter" << endl;
        return;
    }
    ExecuteLogin(usr, passw, id, &mountedPartitions, &currentUser, &activeUser);
}

void ReadMkgrp(vector<string> params){
    string nameGrp = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-name-"){
            nameGrp = ParameterName(value);
            if(nameGrp == ""){
                return;
            }
        }else{
            cout << "$Error: "+name+" is not a valid parameter"<<endl;
            return;
        }
    }

    if(nameGrp == ""){
        cout << "$Error: NAME is a mandatory parameter" << endl;
        return;
    }

    ExecuteMkGrp(nameGrp, &currentUser, &activeUser);
}

void ReadRmgrp(vector<string> params){
    string nameGrp = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-name-"){
            nameGrp = ParameterName(value);
            if(nameGrp == ""){
                return;
            }
        }else{
            cout << "$Error: "+name+" is not a valid parameter"<<endl;
            return;
        }
    }

    if(nameGrp == ""){
        cout << "$Error: NAME is a mandatory parameter" << endl;
        return;
    }

    ExecuteRmgrp(nameGrp, &currentUser, &activeUser);
}

void ReadMkusr(vector<string> params){
    string nameGrp = "", usr = "", passw = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-grp-"){
            nameGrp = ParameterGrp(value);
            if(nameGrp == ""){
                return;
            }
        }else if(name == "-usr-"){
            usr = ParameterCredential(value);
            if(usr == ""){
                return;
            }
        }else if(name == "-pass-"){
            passw = ParameterCredential(value);
            if(passw == ""){
                return;
            }
        }else{
            cout << "$Error: "+name+" is not a valid parameter"<<endl;
            return;
        }
    }

    if(nameGrp == ""){
        cout << "$Error: GRP is a mandatory parameter" << endl;
        return;
    }
    if(passw == ""){
        cout << "$Error: PASS is a mandatory parameter" << endl;
        return;
    }
    if(usr == ""){
        cout << "$Error: USR is a mandatory parameter" << endl;
        return;
    }

    ExecuteMkusr(usr, passw, nameGrp, &currentUser, &activeUser);
}

void ReadRmusr(vector<string> params){
    string nameUsr = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];
        if(name == "-usr-"){
            nameUsr = ParameterCredential(value);
            if(nameUsr == ""){
                return;
            }
        }else{
            cout << "$Error: "+name+" is not a valid parameter"<<endl;
            return;
        }
    }

    if(nameUsr == ""){
        cout << "$Error: USR is a mandatory parameter" << endl;
        return;
    }
    ExecuteRmusr(nameUsr, &currentUser, &activeUser);
}

void ReadChmod(vector<string> params){
    string path = "";
    int ugo = -1;
    bool r = false;

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        if(name == "-r"){
            r = true;
            continue;
        }
        string value = param[1];

        if(name == "-ugo-"){
            ugo = ParameterUGO(value);
            if(ugo == -1){
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
    if(ugo == -1){
        cout << "$Error: UGO is a mandatory parameter" << endl;
        return;
    }
}

void ReadReport(vector<string> params){
    string path = "", reportName = "", id = "", ruta = "";

    for(int i = 0; i < params.size(); i++){
        vector<string> param = Split(params[i], '>');
        string name = ToLower(param[0]);
        string value = param[1];

        if(name == "-name-"){
            reportName = ParameterName(value);
            if(reportName == ""){
                return;
            }
        }else if(name == "-path-"){
            path = ParameterPath(value);
            if(path == ""){
                return;
            }
        }else if(name == "-id-"){
            id = ParameterId(value);
            if(id == ""){
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
    if(id == ""){
        cout << "$Error: ID is a mandatory parameter" << endl;
        return;
    }
    if(reportName == ""){
        cout << "$Error: NAME is a mandatory parameter" << endl;
        return;
    }

    ExecuteReport(id, reportName, path, &mountedPartitions, ruta);
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
    }else if(cmd == "mkfs"){
        ReadMkfs(command);
    }else if(cmd == "login"){
        ReadLogin(command);
    }else if(cmd == "mkgrp"){
        ReadMkgrp(command);
    }else if(cmd == "rmgrp"){
        ReadRmgrp(command);
    }else if(cmd == "mkusr"){
        ReadMkusr(command);
    }else if(cmd == "rmusr"){
        ReadRmusr(command);
    }else if(cmd == "chmod"){
        ReadChmod(command);
    }else if(cmd == "login"){
        ReadLogin(command);
    }else if(cmd == "logout"){
        ExecuteLogout(&currentUser, &activeUser);
    }else if(cmd == "exit"){
        exit(EXIT_SUCCESS);
    }else{
        cout << cmd << " command not recognized" << endl;
    }
}