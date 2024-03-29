//
// Created by esdras-toc on 2/08/22.
//
#include "string"
#include "vector"
#include "algorithm"
#include "iostream"
#include "Structures.h"
#include "string.h"
#include "fstream"
#include "math.h"
#include "filesystem"
using namespace std;

string ToLower(string data);
void Read(string str);

bool Exist(string path){
    FILE *archivo;

    if((archivo = fopen(path.c_str(), "r"))){
        fclose(archivo);
        return true;
    }

    return false;
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::string trim(const std::string& str, const std::string& whitespace = " ") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) return "";
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

string RemoveComment(string line){
    string res="";
    for(int i = 0; i < line.length(); i++){
        if(line[i] == '#'){
            break;
        }
        res += line[i];
    }

    return res;
}

string GetFileName(string path){
    string name = "";
    int state = 0;
    for(int i = path.length()-1; i>=0; i--){
        if(state == 0) {
            if (path[i] == '.') {
                state = 1;
            }
            continue;
        }

        if(path[i] == '/' || path[i] == '\\'){
            break;
        }

        name = path[i] + name;
    }

    return name;
}

const string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf,sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

string removeFileName(string path){
    string p;
    bool add = false;
    for(int i = path.length()-1; i >= 0;i--){
        if(!add){
            if(path[i] == '/'){
                add = true;
            }
            continue;
        }

        p = path[i] + p;
    }

    return p;
}

void ExecuteMkdisk(int size, string unit, char fit, string path){
    if(!(Exist(path))){
        if(!hasEnding(path, ".dsk")){
            cout << "$Error: the file must have the extension .dk"<<endl;
            return;
        }

        string paux = removeFileName(path);

        if(paux != "") {
            system(("sudo mkdir -p \'" + paux + "\'").c_str());
            system(("sudo chmod -R 777 \'" + paux + "\'").c_str());
        }

        FILE *file;
        char buff='\0';
        int signature;
        srand(time(NULL));
        signature = rand();

        int i = 0, tam;
        if(unit == "m"){
            tam = (size*1024*1024)/1024;
        }else if(unit == "k"){
            tam = (size*1024)/1024;
        }else{
            cout << "$Error: the value b is not valid for the unit parameter in mkdisc command" << endl;
            return;
        }

        file = fopen(path.c_str(), "wb");

        //crear mbr

        Mbr mbr;
        mbr.mbr_dsk_signature = signature;
        mbr.mbr_tamano = tam*1024;
        strcpy(mbr.mbr_fecha_creacion, currentDateTime().c_str());

        mbr.mbr_dsk_fit = fit;

        Partition par;
        par.part_start = 0;
        //par.part_start = 144;
        //par.part_size = 1024;
        par.part_size = 0;
        strcpy(par.part_name, "");
        par.part_type = '\0';
        par.part_fit = '\0';
        par.part_status = '\0';
        mbr.mbr_partition_1 = par;

        Partition par2;
        //par2.part_start = 0;
        par2.part_start = 0;
        par2.part_size = 0;
        strcpy(par2.part_name, "");
        par2.part_type = '\0';
        par2.part_fit = '\0';
        par2.part_status = '\0';
        mbr.mbr_partition_2 = par2;

        Partition par3;
        par3.part_start = 0;
        //par3.part_start = 3215;
        //par3.part_size = 1024;
        par3.part_size = 0;
        strcpy(par3.part_name, "");
        par3.part_type = '\0';
        par3.part_fit = '\0';
        par3.part_status = '\0';
        mbr.mbr_partition_3 = par3;

        Partition par4;
        par4.part_start = 0;
        //par4.part_start = 5263;
        //par4.part_size = 2048;
        par4.part_size = 0;
        strcpy(par4.part_name, "");
        par4.part_type = '\0';
        par4.part_fit = '\0';
        par4.part_status = '\0';
        mbr.mbr_partition_4 = par4;

        fseek(file, 0, SEEK_SET);
        fwrite(&mbr, sizeof(Mbr), 1, file);

        i = sizeof(Mbr);
        while(i != tam*1024){
            fwrite(&buff, 1, 1, file);
            i++;
        }

        fclose(file);
        cout << "DISK CREATED SUCCESSFULLY"<<endl;
    }else{
        cout << "$Error: The disk already exist"<< endl;
    }
}

void ExecuteRmdisk(string path){
    if(Exist(path)) {
        string confirmation;
        while (true) {
            cout << "Are you sure you want to erase the disk " + path + "? (y/n)" << endl;
            getline(cin, confirmation);
            confirmation = confirmation;
            if (confirmation == "y" || confirmation == "Y") {
                remove(path.c_str());
                cout << "DISK REMOVED SUCCESSFULLY" << endl;
                break;
            } else if (confirmation == "n" || confirmation == "N"){
                cout << "Warning: delete aborted"<<endl;
                break;
            }
        }
    }else{
        cout << "$Error: "<< path <<" doesn't exist"<<endl;
    }
}

bool compareByStart(const Partition &a, const Partition &b)
{
    return a.part_start < b.part_start;
}

void ExecuteFdiskNewPartition(int size, string unit, string path, string type, char fit, string name){
    if(Exist(path)){
        if(name.length() > 16){
            cout << "$Error: partition name cannot be longer than 16 characters" << endl;
        }

        if(type == "p" || type == "e") {
            FILE *file;
            Mbr mbr;
            file = fopen(path.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fread(&mbr, sizeof(Mbr), 1, file);

            Partition *newPar;
            /*
             * verificamos si hay particiones disponibles
             */
            if (mbr.mbr_partition_1.part_start == 0) {
                newPar = &mbr.mbr_partition_1;
            } else if (mbr.mbr_partition_2.part_start == 0) {
                newPar = &mbr.mbr_partition_2;
            } else if (mbr.mbr_partition_3.part_start == 0) {
                newPar = &mbr.mbr_partition_3;
            } else if (mbr.mbr_partition_4.part_start == 0) {
                newPar = &mbr.mbr_partition_4;
            } else {
                cout << "$Error: maximum partitions reached" << endl;
                fclose(file);
                return;
            }

            if ((mbr.mbr_partition_1.part_name == name) || (mbr.mbr_partition_2.part_name == name)
                || (mbr.mbr_partition_3.part_name == name) || (mbr.mbr_partition_4.part_name == name)) {
                cout << "$Error: a partition with that name already exists" << endl;
                fclose(file);
                return;
            }

            /*
             * Si la particion es extendida, verificamos que aun no exista una extendida
             */
            if(type == "e"){
                if(mbr.mbr_partition_1.part_type == 'e' || mbr.mbr_partition_2.part_type == 'e'
                || mbr.mbr_partition_3.part_type == 'e' || mbr.mbr_partition_4.part_type == 'e'){
                    cout << "$Error: an extended partition already exists" << endl;
                    fclose(file);
                    return;
                }
            }

            vector<Partition> partitions;
            if (mbr.mbr_partition_1.part_start > 0) {
                partitions.push_back(mbr.mbr_partition_1);
            }
            if (mbr.mbr_partition_2.part_start > 0) {
                partitions.push_back(mbr.mbr_partition_2);
            }
            if (mbr.mbr_partition_3.part_start > 0) {
                partitions.push_back(mbr.mbr_partition_3);
            }
            if (mbr.mbr_partition_4.part_start > 0) {
                partitions.push_back(mbr.mbr_partition_4);
            }

            sort(partitions.begin(), partitions.end(), compareByStart);

            /*
             * Encontramos los espacios disponibles
             */
            vector<int> availableSpace, startOfSpace;
            if (partitions.size() > 0) {
                for (int i = 0; i < partitions.size(); i++) {
                    if (i == partitions.size() - 1) {
                        availableSpace.push_back(
                                mbr.mbr_tamano - (partitions.at(i).part_start - 1 + partitions.at(i).part_size));
                        startOfSpace.push_back(partitions.at(i).part_start - 1 + partitions.at(i).part_size);
                        continue;
                    }

                    availableSpace.push_back(partitions.at(i + 1).part_start -
                                             (partitions.at(i).part_start + partitions.at(i).part_size));
                    startOfSpace.push_back(partitions.at(i).part_start + partitions.at(i).part_size);
                }
            } else {
                availableSpace.push_back(mbr.mbr_tamano - sizeof(mbr));
                startOfSpace.push_back(sizeof(mbr));
            }

            int tam;
            if (unit == "m") {
                tam = (size * 1024 * 1024);
            } else if (unit == "k") {
                tam = (size * 1024);
            } else {
                tam = size;
            }

            int index = -1;
            if (mbr.mbr_dsk_fit == 'B') {
                /*
                 * ordenamos los espacios de mayor a menor
                 */
                for (int i = 0; i < availableSpace.size() - 1; i++) {
                    for (int j = 0; j < availableSpace.size() - i - 1; j++) {
                        if (availableSpace.at(j) < availableSpace.at(j + 1)) {
                            int temp = availableSpace.at(j);
                            availableSpace.at(j) = availableSpace.at(j + 1);
                            availableSpace.at(j + 1) = temp;

                            temp = startOfSpace.at(j);
                            startOfSpace.at(j) = startOfSpace.at(j + 1);
                            startOfSpace.at(j + 1) = temp;
                        }
                    }
                }

                for (int i = 0; i < availableSpace.size(); i++) {
                    if (availableSpace.at(i) > tam) {
                        index = i;
                    } else if (availableSpace.at(i) == tam) {
                        index = i;
                        break;
                    } else {
                        break;
                    }
                }
            } else if (mbr.mbr_dsk_fit == 'W') {
                /*
                 * ordenamos los espacios de menor a mayor
                 */
                for (int i = 0; i < availableSpace.size() - 1; i++) {
                    for (int j = 0; j < availableSpace.size() - i - 1; j++) {
                        if (availableSpace.at(j) > availableSpace.at(j + 1)) {
                            int temp = availableSpace.at(j);
                            availableSpace.at(j) = availableSpace.at(j + 1);
                            availableSpace.at(j + 1) = temp;

                            temp = startOfSpace.at(j);
                            startOfSpace.at(j) = startOfSpace.at(j + 1);
                            startOfSpace.at(j + 1) = temp;
                        }
                    }
                }

                for (int i = 0; i < availableSpace.size(); i++) {
                    if (availableSpace.at(i) >= tam) {
                        index = i;
                    }
                }
            } else {
                for (int i = 0; i < availableSpace.size(); i++) {
                    if (availableSpace.at(i) >= tam) {
                        index = i;
                        break;
                    }
                }
            }

            if (index == -1) {
                cout << "$Error: no space available" << endl;
                fclose(file);
                return;
            }

            newPar->part_status = 'A';
            newPar->part_type = type[0];
            newPar->part_fit = fit;
            newPar->part_start = startOfSpace.at(index);
            newPar->part_size = tam;
            for (int i = 0; i < 16; i++) {
                if(i < name.length()) {
                    newPar->part_name[i] = name[i];
                    continue;
                }

                newPar->part_name[i] = '\0';
            }

            fseek(file, 0, SEEK_SET);
            fwrite(&mbr, sizeof(Mbr), 1, file);
            fclose(file);

            cout << "PARTITION CREATED SUCCESSFULLY"<<endl;
        }else{
            FILE *file;
            Mbr mbr;
            file = fopen(path.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fread(&mbr, sizeof(Mbr), 1, file);

            Partition *extendedPartition;

            if(mbr.mbr_partition_1.part_type == 'e'){
                extendedPartition = &mbr.mbr_partition_1;
            }else if(mbr.mbr_partition_2.part_type == 'e'){
                extendedPartition = &mbr.mbr_partition_2;
            }else if(mbr.mbr_partition_3.part_type == 'e'){
                extendedPartition = &mbr.mbr_partition_3;
            }else if(mbr.mbr_partition_4.part_type == 'e'){
                extendedPartition = &mbr.mbr_partition_4;
            }else{
                cout << "$Error: there is no extended partition" << endl;
                fclose(file);
                return;
            }

            fseek(file, extendedPartition->part_start, SEEK_SET);
            Ebr ebr;
            fread(&ebr, sizeof(Ebr), 1, file);

            int tam;
            if (unit == "m") {
                tam = (size * 1024 * 1024);
            } else if (unit == "k") {
                tam = (size * 1024);
            } else {
                tam = size;
            }

            int spaceNeeded = tam + sizeof(Ebr);

            if(ebr.part_start == 0 && ebr.part_next == 0){
                if(extendedPartition->part_size >= spaceNeeded){
                    ebr.part_start = extendedPartition->part_start + sizeof(Ebr);
                    ebr.part_next = -1;
                    ebr.part_fit = fit;
                    ebr.part_s = tam;
                    ebr.part_status = 'A';
                    for (int i = 0; i < name.length(); i++) {
                        if(i<name.length()) {
                            ebr.part_name[i] = name[i];
                            continue;
                        }

                        ebr.part_name[i] = '\0';
                    }

                    fseek(file, extendedPartition->part_start, SEEK_SET);
                    fwrite(&ebr, sizeof(Ebr), 1, file);
                    cout << "PARTITION CREATED SUCCESSFULLY"<<endl;
                    fclose(file);
                    return;
                }else{
                    cout << "$Error: no space available" << endl;
                    fclose(file);
                    return;
                }
            }

            int pointer = extendedPartition->part_start;
            vector<Ebr> partitions;
            vector<int> availableSpace;

            /*
             * metemos las particiones dentro de un vector
             */
            while(pointer < extendedPartition->part_start+extendedPartition->part_size-1){
                Ebr ebr;
                fseek(file, pointer, SEEK_SET);
                fread(&ebr, sizeof(Ebr), 1, file);

                if(ebr.part_next == -1){
                    availableSpace.push_back((extendedPartition->part_start+extendedPartition->part_size)-(ebr.part_start+ebr.part_s));
                    partitions.push_back(ebr);
                    break;
                }

                partitions.push_back(ebr);
                availableSpace.push_back(ebr.part_next - (ebr.part_start+ebr.part_s));

                //pointer = ebr.part_start + ebr.part_s;
                pointer = ebr.part_next;
            }

            if(partitions.size()==23){
                cout << "$Error: maximum of logic partitions reached" << endl;
                fclose(file);
                return;
            }

            /*
             * buscamos el lugar adecuado
             */

            int index = -1;
            if(extendedPartition->part_fit == 'B'){
                int actualSpace=-1;
                for(int i = 0; i < availableSpace.size(); i++){
                    if(availableSpace.at(i) >= spaceNeeded){
                        if(actualSpace == -1){
                            actualSpace = availableSpace.at(i);
                            index = i;
                            continue;
                        }else{
                            if(availableSpace.at(i)<actualSpace){
                                actualSpace = availableSpace.at(i);
                                index = i;
                            }
                            continue;
                        }
                    }
                }
            }else if(extendedPartition->part_fit == 'W'){
                int actualSpace=-1;
                for(int i = 0; i < availableSpace.size(); i++){
                    if(availableSpace.at(i) >= spaceNeeded){
                        if(actualSpace == -1){
                            actualSpace = availableSpace.at(i);
                            index = i;
                            continue;
                        }else{
                            if(availableSpace.at(i)>actualSpace){
                                actualSpace = availableSpace.at(i);
                                index = i;
                            }
                            continue;
                        }
                    }
                }
            }else{
                for(int i = 0; i < availableSpace.size(); i++){
                    if(availableSpace.at(i) >= spaceNeeded){
                        index = i;
                        break;
                    }
                }
            }

            if (index == -1) {
                cout << "$Error: no space available" << endl;
                fclose(file);
                return;
            }

            ebr.part_start = partitions.at(index).part_start + partitions.at(index).part_s + sizeof(Ebr);
            ebr.part_next = -1;
            if(index < availableSpace.size()-1) {
                ebr.part_next = partitions.at(index).part_next;
            }
            ebr.part_fit = fit;
            ebr.part_s = tam;
            ebr.part_status = 'A';
            for (int i = 0; i < name.length(); i++) { ebr.part_name[i] = name[i]; }

            partitions.at(index).part_next = ebr.part_start - sizeof(Ebr);

            fseek(file, partitions.at(index).part_start - sizeof(Ebr), SEEK_SET);
            fwrite(&(partitions.at(index)), sizeof(Ebr), 1, file);

            fseek(file, ebr.part_start- sizeof(Ebr), SEEK_SET);
            fwrite(&ebr, sizeof(Ebr), 1, file);


            cout << "PARTITION CREATED SUCCESSFULLY"<<endl;
            fclose(file);
        }
    }else{
        cout << "$Error: "<< path <<" doesn't exist"<<endl;
    }
}

void ExecuteFdiskAddPartition(int add, string unit, string path, string nameString){
    if(Exist(path)){
        char name[16];
        for(int i = 0; i < 16; i++){
            if(i < nameString.length()) {
                name[i] = nameString[i];
                continue;
            }
            name[i] = '\0';
        }
        int tam;
        if (unit == "m") {
            tam = (add * 1024 * 1024);
        } else if (unit == "k") {
            tam = (add * 1024);
        } else {
            tam = add;
        }

        FILE *file;
        Mbr mbr;
        file = fopen(path.c_str(), "rb+");
        fseek(file, 0, SEEK_SET);
        fread(&mbr, sizeof(Mbr), 1, file);

        Partition *par;
        bool found = false;

        if(ToLower(mbr.mbr_partition_1.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_1;
            found = true;
        }else if(ToLower(mbr.mbr_partition_2.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_2;
            found = true;
        }else if(ToLower(mbr.mbr_partition_3.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_3;
            found = true;
        }else if(ToLower(mbr.mbr_partition_4.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_4;
            found = true;
        }

        // Es primaria o extendida
        if(found){
            //restar espacio
            if(tam < 0){
                if(-1*tam <= par->part_size){
                    par->part_size += tam;
                }else{
                    fclose(file);
                    cout << "$Error: the partition does not have enough space" << endl;
                    return;
                }
            }else {
                vector<Partition> partitions;
                if (mbr.mbr_partition_1.part_start > 0) {
                    partitions.push_back(mbr.mbr_partition_1);
                }
                if (mbr.mbr_partition_2.part_start > 0) {
                    partitions.push_back(mbr.mbr_partition_2);
                }
                if (mbr.mbr_partition_3.part_start > 0) {
                    partitions.push_back(mbr.mbr_partition_3);
                }
                if (mbr.mbr_partition_4.part_start > 0) {
                    partitions.push_back(mbr.mbr_partition_4);
                }

                sort(partitions.begin(), partitions.end(), compareByStart);

                int i;
                for(i = 0; i < partitions.size(); i++){
                    if(partitions.at(i).part_start == par->part_start){
                        break;
                    }
                }

                int availableSpace;
                if(i == partitions.size()-1){
                    availableSpace = mbr.mbr_tamano - (par->part_start+par->part_size-1);
                }else{
                    availableSpace = partitions.at(i+1).part_start-(par->part_start+par->part_size-1);
                }

                if(availableSpace >= tam){
                    par->part_size += tam;
                }else{
                    fclose(file);
                    cout << "$Error: not enough space" << endl;
                    return;
                }
            }

            fseek(file, 0, SEEK_SET);
            fwrite(&mbr, sizeof(Mbr), 1, file);

            cout << "SPACE MODIFICATION COMPLETED"<<endl;
            fclose(file);
            return;
        }

        //buscar en logicas
        if(mbr.mbr_partition_1.part_type == 'e'){
            par = &mbr.mbr_partition_1;
        }else if(mbr.mbr_partition_2.part_type == 'e'){
            par = &mbr.mbr_partition_2;
        }else if(mbr.mbr_partition_3.part_type == 'e'){
            par = &mbr.mbr_partition_3;
        }else if(mbr.mbr_partition_4.part_type == 'e'){
            par = &mbr.mbr_partition_4;
        }else{
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        Ebr ebr;
        fseek(file, par->part_start, SEEK_SET);
        fread(&ebr, sizeof(Ebr), 1, file);

        if(ebr.part_start == 0 && ebr.part_next == 0){
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        int pointer = par->part_start;
        vector<Ebr> partitions;
        vector<int> availableSpace;
        found = false;
        int index = 0;

        /*
         * metemos las particiones dentro de un vector
         */
        while(pointer < par->part_start+par->part_size-1){
            Ebr ebr;
            fseek(file, pointer, SEEK_SET);
            fread(&ebr, sizeof(Ebr), 1, file);

            if(ToLower(ebr.part_name) == ToLower(name)){
                found = true;
                index = partitions.size();
            }

            if(ebr.part_next == -1){
                availableSpace.push_back((par->part_start+par->part_size)-(ebr.part_start+ebr.part_s));
                partitions.push_back(ebr);
                break;
            }

            partitions.push_back(ebr);
            availableSpace.push_back(ebr.part_next - (ebr.part_start+ebr.part_s));

            pointer = ebr.part_next;
        }

        if(!found){
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        if(tam < 0){
            if(partitions.at(index).part_s >= tam*-1){
                partitions.at(index).part_s+=tam;
            }else{
                fclose(file);
                cout << "$Error: the partition does not have enough space" << endl;
                return;
            }
        }else {
            if(tam <= availableSpace.at(index)){
                partitions.at(index).part_s+=tam;
            }else{
                fclose(file);
                cout << "$Error: not enough space" << endl;
                return;
            }
        }

        fseek(file, partitions.at(index).part_start- sizeof(Ebr), SEEK_SET);
        fwrite(&partitions.at(index), sizeof(Ebr), 1, file);

        cout << "SPACE MODIFICATION COMPLETED"<<endl;
        fclose(file);
        return;
    }else{
        cout << "$Error: "<< path <<" doesn't exist"<<endl;
    }
}

void ExecuteFdiskDeletePartition(string path, string nameString){
    if(Exist(path)){
        char name[16];
        for(int i = 0; i < 16; i++){
            if(i < nameString.length()) {
                name[i] = nameString[i];
                continue;
            }
            name[i] = '\0';
        }

        FILE *file;
        Mbr mbr;
        file = fopen(path.c_str(), "rb+");
        fseek(file, 0, SEEK_SET);
        fread(&mbr, sizeof(Mbr), 1, file);

        Partition *par;
        bool found = false;

        if(ToLower(mbr.mbr_partition_1.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_1;
            found = true;
        }else if(ToLower(mbr.mbr_partition_2.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_2;
            found = true;
        }else if(ToLower(mbr.mbr_partition_3.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_3;
            found = true;
        }else if(ToLower(mbr.mbr_partition_4.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_4;
            found = true;
        }

        // Es primaria o extendida
        if(found){
            char c = '\0';
            for(int i = par->part_start; i < par->part_start+par->part_size;i++){
                fseek(file, i, SEEK_SET);
                fwrite(&c, 1, 1, file);
            }

            par->part_start = 0;
            par->part_size = 0;
            par->part_type = '\0';
            for(int i = 0; i<16; i++){par->part_name[i] = '\0';}
            //strcpy(par->part_name, "");
            par->part_fit = '\0';
            par->part_status = '\0';


            fseek(file, 0, SEEK_SET);
            fwrite(&mbr, sizeof(Mbr), 1, file);

            cout << name << " DELETED"<<endl;
            fclose(file);
            return;
        }

        //buscar en logicas
        if(mbr.mbr_partition_1.part_type == 'e'){
            par = &mbr.mbr_partition_1;
        }else if(mbr.mbr_partition_2.part_type == 'e'){
            par = &mbr.mbr_partition_2;
        }else if(mbr.mbr_partition_3.part_type == 'e'){
            par = &mbr.mbr_partition_3;
        }else if(mbr.mbr_partition_4.part_type == 'e'){
            par = &mbr.mbr_partition_4;
        }else{
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        Ebr ebr;
        fseek(file, par->part_start, SEEK_SET);
        fread(&ebr, sizeof(Ebr), 1, file);

        if(ebr.part_start == 0 && ebr.part_next == 0){
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        int pointer = par->part_start;
        vector<Ebr> partitions;
        found = false;
        int index = 0;

        /*
         * metemos las particiones dentro de un vector
         */
        while(pointer < par->part_start+par->part_size-1){
            Ebr ebr;
            fseek(file, pointer, SEEK_SET);
            fread(&ebr, sizeof(Ebr), 1, file);

            if(ToLower(ebr.part_name) == ToLower(name)){
                found = true;
                index = partitions.size();
            }

            if(ebr.part_next == -1){
                partitions.push_back(ebr);
                break;
            }

            partitions.push_back(ebr);

            pointer = ebr.part_next;
        }

        if(!found){
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        if(index == partitions.size()-1){
            partitions.at(index-1).part_next = -1;
        }else if(index == 0){
            cout << "THE FIRS LOGIC PARTITION CANNOT BE DELETED" << endl;
            fclose(file);
            return;
        }else{
            partitions.at(index-1).part_next = partitions.at(index).part_next;
        }

        fseek(file, partitions.at(index-1).part_start - sizeof(Ebr), SEEK_SET);
        fwrite(&partitions.at(index-1), sizeof(Ebr), 1, file);

        char c = '\0';
        for(int i = partitions.at(index).part_start - sizeof(Ebr);
            i < partitions.at(index).part_start+partitions.at(index).part_s;i++){
            fseek(file, i, SEEK_SET);
            fwrite(&c, 1, 1, file);
        }

        cout << name << " DELETED"<<endl;

        fclose(file);
        return;
    }else{
        cout << "$Error: "<< path <<" doesn't exist"<<endl;
    }
}

void ExecuteMount(string p, string nameString, vector<MountedPartition> *partitions){
    if(Exist(p)){
        char name[16];
        for(int i = 0; i < 16; i++){
            if(i < nameString.length()) {
                name[i] = nameString[i];
                continue;
            }
            name[i] = '\0';
        }

        FILE *file;
        Mbr mbr;
        file = fopen(p.c_str(), "rb+");
        fseek(file, 0, SEEK_SET);
        fread(&mbr, sizeof(Mbr), 1, file);

        Partition *par;
        bool found = false;
        int partitionNumber = 0;

        if(ToLower(mbr.mbr_partition_1.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_1;
            found = true;
            partitionNumber = 1;
        }else if(ToLower(mbr.mbr_partition_2.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_2;
            found = true;
            partitionNumber = 2;
        }else if(ToLower(mbr.mbr_partition_3.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_3;
            found = true;
            partitionNumber = 3;
        }else if(ToLower(mbr.mbr_partition_4.part_name) == ToLower(name)){
            par = &mbr.mbr_partition_4;
            found = true;
            partitionNumber = 4;
        }

        int numberOfPartitions=0;
        if(mbr.mbr_partition_1.part_start != 0){
            numberOfPartitions++;
        }
        if(mbr.mbr_partition_2.part_start != 0){
            numberOfPartitions++;
        }
        if(mbr.mbr_partition_3.part_start != 0){
            numberOfPartitions++;
        }
        if(mbr.mbr_partition_4.part_start != 0){
            numberOfPartitions++;
        }

        if(found){
            if(par->part_type == 'e'){
                cout << "$Error: extender partitions cannot be mounted"<<endl;
                fclose(file);
                return;
            }
            MountedPartition newMount;
            newMount.par = *par;
            newMount.isLogic = false;
            newMount.path = p;
            newMount.id = "73"+ to_string(partitionNumber) + GetFileName(p);

            partitions->push_back(newMount);
            cout << "PATITION MOUNTED, ID -> " << newMount.id <<endl;

            fseek(file, par->part_start, SEEK_SET);
            SuperBlock sp;
            fread(&sp, sizeof(SuperBlock), 1, file);

            if(sp.s_filesystem_type != 0){
                sp.s_mnt_count += 1;
                strcpy(sp.s_mtime, currentDateTime().c_str());

                fseek(file, par->part_start, SEEK_SET);
                fwrite(&sp, sizeof(SuperBlock), 1, file);
            }

            fclose(file);
            return;
        }

        //buscar en logicas
        if(mbr.mbr_partition_1.part_type == 'e'){
            par = &mbr.mbr_partition_1;
        }else if(mbr.mbr_partition_2.part_type == 'e'){
            par = &mbr.mbr_partition_2;
        }else if(mbr.mbr_partition_3.part_type == 'e'){
            par = &mbr.mbr_partition_3;
        }else if(mbr.mbr_partition_4.part_type == 'e'){
            par = &mbr.mbr_partition_4;
        }else{
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        Ebr ebr;
        fseek(file, par->part_start, SEEK_SET);
        fread(&ebr, sizeof(Ebr), 1, file);

        if(ebr.part_start == 0 && ebr.part_next == 0){
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        int pointer = par->part_start;
        vector<Ebr> logicpartitions;
        found = false;
        int index = 0;

        /*
         * metemos las particiones dentro de un vector
         */
        while(pointer < par->part_start+par->part_size-1){
            Ebr ebr;
            fseek(file, pointer, SEEK_SET);
            fread(&ebr, sizeof(Ebr), 1, file);

            if(ToLower(ebr.part_name) == ToLower(name)){
                found = true;
                index = logicpartitions.size();
            }

            numberOfPartitions++;
            if(ebr.part_next == -1){
                logicpartitions.push_back(ebr);
                break;
            }

            logicpartitions.push_back(ebr);
            pointer = ebr.part_next;
        }

        if(!found){
            fclose(file);
            cout << "$Error: the partition doesn't exist"<<endl;
            return;
        }

        MountedPartition newMount;
        newMount.logicPar = logicpartitions.at(index);
        newMount.isLogic = true;
        newMount.path = p;
        newMount.id = "73"+ to_string(numberOfPartitions) + GetFileName(p);

        fseek(file, logicpartitions.at(index).part_start, SEEK_SET);
        SuperBlock sp;
        fread(&sp, sizeof(SuperBlock), 1, file);

        if(sp.s_filesystem_type != 0){
            sp.s_mnt_count += 1;
            strcpy(sp.s_mtime, currentDateTime().c_str());

            fseek(file, logicpartitions.at(index).part_start, SEEK_SET);
            fwrite(&sp, sizeof(SuperBlock), 1, file);
        }

        partitions->push_back(newMount);
        cout << "PATITION MOUNTED, ID -> " << newMount.id <<endl;
        fclose(file);
        return;
    }else{
        cout << "$Error: "<< p <<" doesn't exist"<<endl;
    }
}

void ExecuteUnmount(string id, vector<MountedPartition> *partitions){
    for(int i = 0; i < partitions->size(); i++){
        if(partitions->at(i).id == id){

            SuperBlock sp;
            FILE *file;
            file = fopen(partitions->at(i).path.c_str(), "rb+");
            int pointer = 0;
            if(partitions->at(i).isLogic) {
                pointer = partitions->at(i).logicPar.part_start;
            }else{
                pointer = partitions->at(i).par.part_start;
            }

            fseek(file, pointer, SEEK_SET);
            fread(&sp, sizeof(SuperBlock), 1, file);

            if(sp.s_filesystem_type != 0){
                strcpy(sp.s_umtime, currentDateTime().c_str());
                fseek(file, pointer, SEEK_SET);
                fwrite(&sp, sizeof(SuperBlock), 1, file);
            }

            partitions->erase(partitions->begin()+i);
            cout << "THE PARTITION WAS UNMOUNTED SUCCESSFULLY"<<endl;
            fclose(file);
            return;
        }
    }

    cout << "$Error: "<<id<<" is not mounted"<<endl;
}

void ExecuteExec(string path){
    if(Exist(path)){
        if(!hasEnding(path, ".mia")){
            cout << "$Error: the file must have the extension .mia"<<endl;
            return;
        }

        fstream file;
        file.open(path, ios::in);

        if(file.is_open()){
            string line;
            string finalLine;
            while(getline(file, line)){
                cout << line << endl;
                finalLine = RemoveComment(line);
                finalLine = trim(finalLine);
                finalLine = trim(finalLine, "\t");
                if(finalLine != ""){
                    Read(finalLine);
                }
            }
        }
    }else{
        cout << "$Error: "<< path <<" doesn't exist"<<endl;
    }
}

void ReportMbr(string path, string imagePath);
void ReportBMInode(MountedPartition *mountedPartition, string path);
void ReportBMBlocks(MountedPartition *mountedPartition, string path);
void ReportInodes(MountedPartition partition, string path);
void ReportTree(MountedPartition partition, string path);
void ReportJournaling(MountedPartition partition, string path);
void ReportFile(Sesion currentUser, string reportPath, string filePath);
void ReportSb(MountedPartition partition, string path);

void ExecuteReport(string id, string name, string path, vector<MountedPartition> *partitions, string ruta, Sesion currentUser){
    MountedPartition *mountedPartition;
    int i = 0;
    for(i = 0; i < partitions->size(); i++){
        if(id == partitions->at(i).id){
            mountedPartition = &(partitions->at(i));
            break;
        }
    }

    if(i == partitions->size()){
        cout << "$Error: "<<id<<" is not mounted"<<endl;
        return;
    }

    if(ToLower(name) == "mbr"){
        ReportMbr(mountedPartition->path, path);
    }else if(ToLower(name) == "bm_inode"){
        ReportBMInode(mountedPartition, path);
    }else if(ToLower(name) == "bm_block"){
        ReportBMBlocks(mountedPartition, path);
    }else if(ToLower(name) == "inode"){
        ReportInodes(*mountedPartition, path);
    }else if(ToLower(name) == "tree"){
        ReportTree(*mountedPartition, path);
    }else if(ToLower(name) == "journaling"){
        ReportJournaling(*mountedPartition, path);
    }else if(ToLower(name) == "sb"){
        ReportSb(*mountedPartition, path);
    }else if(ToLower(name) == "file"){
        if(ruta == ""){
            cout << "$Error: RUTA is a mandatory parameter with file and ls reports"<<endl;
            return;
        }
        ReportFile(currentUser, path, ruta);
    }
}

Inode newInode(){
    Inode inode;
    char bufferDate[19];

    for(int i = 0; i < 19; i++){
        bufferDate[i] = '\0';
    }

    inode.i_uid = 1;
    inode.i_gid = 1;
    inode.i_s = 0;
    strcpy(inode.i_atime, bufferDate);
    strcpy(inode.i_ctime, currentDateTime().c_str());
    strcpy(inode.i_mtime, currentDateTime().c_str());
    for(int i = 0; i < 15; i++){
        inode.i_block[i] = -1;
    }
    inode.i_type = '0';
    inode.i_perm = 0;

    return inode;
}

void ext2(MountedPartition *mountedPartition){
    int sizeOfPartition, start;
    if(mountedPartition->isLogic){
        sizeOfPartition = mountedPartition->logicPar.part_s;
        start = mountedPartition->logicPar.part_start;
    }else{
        sizeOfPartition = mountedPartition->par.part_size;
        start = mountedPartition->par.part_start;
    }

    int num_structures = floor((sizeOfPartition - sizeof(SuperBlock))/(4 + sizeof(Inode)+ 3*sizeof(FileBlock)));
    int num_blocks = 3*num_structures;

    SuperBlock sp;
    sp.s_filesystem_type = 2;
    sp.s_inodes_count = num_structures;
    sp.s_blocks_count = num_blocks;
    sp.s_free_blocks_counts = num_blocks - 2;
    sp.s_free_inodes_count = num_structures - 2;
    sp.s_mnt_count = 0;
    sp.s_magic = 0xEF53;
    sp.s_inode_s = sizeof(Inode);
    sp.s_block_s = sizeof(FileBlock);
    sp.s_first_ino = 2;
    sp.s_first_blo = 2;
    sp.s_bm_inode_start = start + sizeof(SuperBlock);
    sp.s_bm_block_start = sp.s_bm_inode_start + num_structures;
    sp.s_inode_start = sp.s_bm_block_start + num_blocks;
    sp.s_block_start = sp.s_inode_start + (num_structures * sizeof(Inode));

    FILE *file;
    file = fopen(mountedPartition->path.c_str(), "rb+");
    //Creando la estructura:
    //   creamos el super bloque
    fseek(file, start, SEEK_SET);
    fwrite(&sp, sizeof(SuperBlock), 1, file);

    // creamos el bitmap de inodos pero previamente colocamos el inodo de / y de users.txt
    char b1 = '1';
    fwrite(&b1, 1, 1, file);
    fwrite(&b1, 1, 1, file);
    char b0 = '0';
    for(int i = 2; i < num_structures; i++){
        fwrite(&b0, 1, 1, file);
    }

    // creamos el bitmap de bloques los bloques para / y users.txt
    fwrite(&b1, 1, 1, file);
    fwrite(&b1, 1, 1, file);
    for(int i = 2; i < num_blocks; i++){
        fwrite(&b0, 1, 1, file);
    }

    //creamos e insertamos el primer inodo de /
    Inode inode = newInode();
    inode.i_block[0] = 0;
    inode.i_perm = 664;

    fwrite(&inode, sizeof(Inode), 1, file);

    //creamos el inodo para users.txt
    inode = newInode();
    inode.i_s = 27;
    inode.i_block[0] = 1;
    inode.i_type = '1';
    inode.i_perm = 755;

    fwrite(&inode, sizeof(Inode), 1, file);
    //creamos el bloque para la carpeta /
    DirBlock dirBlock;

    strcpy(dirBlock.b_content[0].b_name, ".");
    dirBlock.b_content[0].b_inodo = 0;
    strcpy(dirBlock.b_content[1].b_name, "..");
    dirBlock.b_content[1].b_inodo = 0;
    strcpy(dirBlock.b_content[2].b_name, "users.txt");
    dirBlock.b_content[2].b_inodo = 1;

    strcpy(dirBlock.b_content[3].b_name, "\0");
    dirBlock.b_content[3].b_inodo = -1;

    fseek(file, sp.s_block_start, SEEK_SET);
    fwrite(&dirBlock, sizeof(DirBlock), 1, file);

    // creamos el bloque para users.txt
    FileBlock fileBlock;
    memset(fileBlock.b_content, 0, sizeof(fileBlock.b_content));
    strcpy(fileBlock.b_content, "1,G,root\n1,U,root,root,123\n");
    fwrite(&fileBlock, sizeof(FileBlock), 1, file);

    fclose(file);
    cout << "EXT2 FORMAT DONE SUCCESFULLY"<<endl;

    return;
}

void ext3(MountedPartition *mountedPartition) {
    int sizeOfPartition, start;
    if (mountedPartition->isLogic) {
        sizeOfPartition = mountedPartition->logicPar.part_s;
        start = mountedPartition->logicPar.part_start;
    } else {
        sizeOfPartition = mountedPartition->par.part_size;
        start = mountedPartition->par.part_start;
    }

    int num_structures = floor((sizeOfPartition - sizeof(SuperBlock))/(4 + sizeof(Inode)+ 3*sizeof(FileBlock) + sizeof(Journal)));
    int num_blocks = 3*num_structures;

    SuperBlock sp;
    sp.s_filesystem_type = 3;
    sp.s_inodes_count = num_structures;
    sp.s_blocks_count = num_blocks;
    sp.s_free_blocks_counts = num_blocks - 2;
    sp.s_free_inodes_count = num_structures - 2;
    sp.s_mnt_count = 0;
    sp.s_magic = 0xEF53;
    sp.s_inode_s = sizeof(Inode);
    sp.s_block_s = sizeof(FileBlock);
    sp.s_first_ino = 2;
    sp.s_first_blo = 2;
    sp.s_bm_inode_start = start + sizeof(SuperBlock) + (sizeof(Journal)*num_structures);
    sp.s_bm_block_start = sp.s_bm_inode_start + num_structures;
    sp.s_inode_start = sp.s_bm_block_start + num_blocks;
    sp.s_block_start = sp.s_inode_start + (num_structures * sizeof(Inode));

    FILE *file;
    file = fopen(mountedPartition->path.c_str(), "rb+");
    //Creando la estructura:
    //   creamos el super bloque
    fseek(file, start, SEEK_SET);
    fwrite(&sp, sizeof(SuperBlock), 1, file);

    //dejamos el espacio para el journal
    fseek(file, start + sizeof(SuperBlock), SEEK_SET);

    for(int i = 0; i < num_structures;i++){
        Journal j;
        j.j_type = -1;
        fwrite(&j, sizeof(Journal), 1, file);
    }

    // creamos el bitmap de inodos pero previamente colocamos el inodo de / y de users.txt
    char b1 = '1';
    fwrite(&b1, 1, 1, file);
    fwrite(&b1, 1, 1, file);
    char b0 = '0';
    for(int i = 2; i < num_structures; i++){
        fwrite(&b0, 1, 1, file);
    }

    // creamos el bitmap de bloques los bloques para / y users.txt
    fwrite(&b1, 1, 1, file);
    fwrite(&b1, 1, 1, file);
    for(int i = 2; i < num_blocks; i++){
        fwrite(&b0, 1, 1, file);
    }

    //creamos e insertamos el primer inodo de /
    Inode inode = newInode();
    inode.i_block[0] = 0;
    inode.i_perm = 664;

    fwrite(&inode, sizeof(Inode), 1, file);

    //creamos el inodo para users.txt
    inode = newInode();
    inode.i_s = 27;
    inode.i_block[0] = 1;
    inode.i_type = '1';
    inode.i_perm = 755;

    fwrite(&inode, sizeof(Inode), 1, file);
    //creamos el bloque para la carpeta /
    DirBlock dirBlock;

    strcpy(dirBlock.b_content[0].b_name, ".");
    dirBlock.b_content[0].b_inodo = 0;
    strcpy(dirBlock.b_content[1].b_name, "..");
    dirBlock.b_content[1].b_inodo = 0;
    strcpy(dirBlock.b_content[2].b_name, "users.txt");
    dirBlock.b_content[2].b_inodo = 1;

    strcpy(dirBlock.b_content[3].b_name, "\0");
    dirBlock.b_content[3].b_inodo = -1;

    fseek(file, sp.s_block_start, SEEK_SET);
    fwrite(&dirBlock, sizeof(DirBlock), 1, file);

    // creamos el bloque para users.txt
    FileBlock fileBlock;
    memset(fileBlock.b_content, 0, sizeof(fileBlock.b_content));
    strcpy(fileBlock.b_content, "1,G,root\n1,U,root,root,123\n");
    fwrite(&fileBlock, sizeof(FileBlock), 1, file);

    fclose(file);
    cout << "EXT3 FORMAT DONE SUCCESFULLY"<<endl;

    return;
}

void writeJournal(int start, SuperBlock sb, string cmd, int type, string path, string content, int userId, int perm, FILE *file){
    Journal j;
    int i = 0;
    for(i = start + sizeof(SuperBlock); i < sb.s_bm_inode_start; i += sizeof(Journal)){
        fseek(file, i, SEEK_SET);
        fread(&j, sizeof(Journal), 1, file);

        if(j.j_type == -1){
            break;
        }
    }

    if(i >= sb.s_bm_inode_start){
        cout << "$Error: no more journals available"<<endl;
        return;
    }

    strcpy(j.j_operation, cmd.c_str());
    strcpy(j.j_content, content.c_str());
    strcpy(j.j_date, currentDateTime().c_str());
    strcpy(j.j_name, path.c_str());
    j.j_type = type;
    j.j_owner = userId;

    fseek(file, i , SEEK_SET);
    fwrite(&j, sizeof(Journal), 1, file);
}

void ExecuteMkfs(string id, int fs, vector<MountedPartition> *partitions){
    MountedPartition *mountedPartition;
    int i = 0;
    for(i = 0; i < partitions->size(); i++){
        if(id == partitions->at(i).id){
            mountedPartition = &(partitions->at(i));
            break;
        }
    }

    if(i == partitions->size()){
        cout << "$Error: "<<id<<" is not mounted"<<endl;
        return;
    }

    if(fs == 2){
        ext2(mountedPartition);
    }else{
        ext3(mountedPartition);
    }
}

vector<string> splitPath(string path){
    vector<string> res;
    string aux = "";
    for(int i = 0; i < path.length(); i++){
        if(path[i] == '/'){
            if(i == 0){
                continue;
            }
            res.push_back(aux);
            aux = "";
            continue;
        }

        aux += path[i];

        if(i == path.length()-1){
            res.push_back(aux);
        }
    }

    return res;
}

Inode searchInDirBlocks(int pointer, FILE *file, string name, int istart, int bstart, int *p){
    DirBlock dirBlock;
    Inode res;
    res.i_type = 'n';

    if(pointer == -1){
        return res;
    }

    pointer = bstart+(pointer*64);
    fseek(file, pointer, SEEK_SET);
    fread(&dirBlock, sizeof(DirBlock), 1, file);
    for(int i = 0; i < 4; i++){
        if(dirBlock.b_content[i].b_name == name){
            fseek(file, istart+(dirBlock.b_content[i].b_inodo * sizeof(Inode)), SEEK_SET);
            fread(&res, sizeof(Inode), 1, file);
            *p = dirBlock.b_content[i].b_inodo;
            return res;
        }
    }
    return res;
}

Inode searchFileInPointerBlocks(int dim, string name, FILE *file, int pointer, int istart, int bstart, int *p){
    Inode res;
    res.i_type = 'n';

    if(pointer == -1){
        return res;
    }

    pointer = bstart+(pointer*64);
    PointerBlock pb;
    fseek(file, pointer, SEEK_SET);
    fread(&pb, sizeof(PointerBlock), 1, file);
    if(dim == 1){
        for(int i = 0; i < 16; i++){
            res = searchInDirBlocks(pb.b_pointers[i], file, name, istart, bstart, p);
            if(res.i_type != 'n'){
                return res;
            }
        }
    }else{
        for(int i = 0; i < 16; i++){
            res = searchFileInPointerBlocks(dim-1, name, file, pb.b_pointers[i], istart, bstart, p);
            if(res.i_type != 'n'){
                return res;
            }
        }
    }
    return res;
}

string readInFileBlocks(int pointer, FILE *file, int istart, int bstart){
    FileBlock dirBlock;
    string res = "";

    if(pointer == -1){
        return res;
    }

    pointer = bstart+(pointer*64);
    fseek(file, pointer, SEEK_SET);
    fread(&dirBlock, sizeof(FileBlock), 1, file);
    res += dirBlock.b_content;
    return res;
}

string readFileInPointerBlocks(int dim, FILE *file, int pointer, int istart, int bstart){
    string res = "";
    if(pointer == -1){
        return res;
    }

    pointer = bstart+(pointer*64);
    PointerBlock pb;
    fseek(file, pointer, SEEK_SET);
    fread(&pb, sizeof(PointerBlock), 1, file);
    if(dim == 1){
        for(int i = 0; i < 16; i++){
            res += readInFileBlocks(pb.b_pointers[i], file, istart, bstart);
        }
    }else{
        for(int i = 0; i < 16; i++){
            res += readFileInPointerBlocks(dim-1,file, pb.b_pointers[i], istart, bstart);
        }
    }
    return res;
}

Inode searchFile(FILE *file, Inode inode, vector<string> path, int istart, int bstart, int *p){
    int pointer;
    Inode res;
    res.i_type = 'n';
    for(int i = 0; i < path.size(); i++){
        if(inode.i_type == '0'){
            for(int j = 0; j < 15; j++){
                if(j < 12){
                    res = searchInDirBlocks(inode.i_block[j], file, path.at(i), istart, bstart, p);
                }else{
                    res = searchFileInPointerBlocks(j-11, path.at(i), file, inode.i_block[j], istart, bstart, p);
                }

                if(res.i_type != 'n'){
                    inode = res;
                    break;
                }
            }

            if(res.i_type == 'n'){
                break;
            }
        }else{
            res.i_type = 'n';
            return res;
        }
    }
    return res;
}

void changeUGO(FILE *file, Inode inode, vector<string> path, int istart, int bstart, int ugo){
    string auxPath = "";
    Inode found;
    int p = 0;
    for(int i = 0; i < path.size(); i++){
        auxPath += "/"+path.at(i);
        found = searchFile(file, inode, splitPath(auxPath), istart, bstart, &p);

        found.i_perm = ugo;
        strcpy(found.i_mtime, currentDateTime().c_str());

        fseek(file, istart + (p * sizeof(Inode)), SEEK_SET);
        fwrite(&found, sizeof(Inode), 1, file);
    }
}

string readFile(FILE *file, Inode inode, int istart, int bstart){
    int pointer;
    string res = "";

    if(inode.i_type == '1'){
        for(int j = 0; j < 15; j++){
            if(j < 12){
                res += readInFileBlocks(inode.i_block[j], file, istart, bstart);
            }else{
                res += readFileInPointerBlocks(j-11, file, inode.i_block[j], istart, bstart);
            }
        }
    }else{
        res = nullptr;
        cout << "$Error: yo can not read a directory" << endl;
        return res;
    }
    return res;
}

vector<string> getLines(string content){
    vector<string> res;
    string aux = "";
    for(int i = 0; i < content.length(); i++){
        if(content[i] == '\n'){
            res.push_back(aux);
            aux = "";
            continue;
        }
        aux += content[i];
    }

    return res;
}

bool isUser(string line){
    int counter = 0;
    for(int i = 0; i < line.length(); i++){
        if(line[i] == ','){
            counter++;
        }
    }

    return counter == 4;
}

bool authenticate(string usr, string passw, Sesion *currentUser, string content){
    vector<string> usersContent = getLines(content);
    for(int i = 0; i < usersContent.size(); i++){
        if(isUser(usersContent.at(i))){
            int id, counter = 0;
            string group, user, pass, aux = "";
            for(int j = 0; j < usersContent.at(i).length(); j++){
                if(usersContent.at(i)[j] == ',' || j == usersContent.at(i).length()-1){
                    switch (counter) {
                        case 0:
                            id = stoi(aux);
                            break;
                        case 2:
                            group = aux;
                            break;
                        case 3:
                            user = aux;
                            break;
                        case 4:
                            aux += usersContent.at(i)[j];
                            pass = aux;
                            break;
                    }
                    aux = "";
                    counter++;
                    continue;
                }
                aux += usersContent.at(i)[j];
            }

            if(usr == user){
                if(pass == passw){
                    if(id == 0){
                        cout << "$Error: user does not exist" << endl;
                        return false;
                    }
                    strcpy(currentUser->user.group, group.c_str());
                    currentUser->user.id = id;
                    strcpy(currentUser->user.name, user.c_str());
                    strcpy(currentUser->user.password, pass.c_str());
                    return true;
                }
                cout << "$Error: incorrect password" << endl;
                return false;
            }
        }
    }

    cout << "$Error: incorrect user" << endl;
    return false;
}

void ExecuteLogin(string usr, string passw, string id, vector<MountedPartition> *partitions, Sesion *currentUser, bool *activeSession){
    if(*activeSession){
        cout << "$Error: active session" << endl;
        return;
    }

    int i = 0;
    MountedPartition *mountedPartition;
    for(i = 0; i < partitions->size(); i++){
        if(id == partitions->at(i).id){
            mountedPartition = &(partitions->at(i));
            break;
        }
    }

    if(i == partitions->size()){
        cout << "$Error: "<<id<<" is not mounted"<<endl;
        return;
    }

    FILE *file = fopen(mountedPartition->path.c_str(), "rb+");

    SuperBlock sp;
    int start;
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
    }else{
        start = mountedPartition->par.part_start;
    }

    fseek(file, start, SEEK_SET);
    fread(&sp, sizeof(SuperBlock), 1, file);

    Inode root;
    fseek(file, sp.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);

    int p = 0;
    root = searchFile(file, root, splitPath("users.txt"), sp.s_inode_start, sp.s_block_start, &p);

    if(root.i_type == 'n'){
        fclose(file);
        cout << "$Error: users.txt does not exist" << endl;
        return;
    }

    string content = readFile(file, root, sp.s_inode_start, sp.s_block_start);
    if(content.empty()){
        fclose(file);
        return;
    }

    *activeSession = authenticate(usr, passw, currentUser, content);

    if(*activeSession){
        currentUser->mountedPartition = *mountedPartition;
        cout << "WELCOME " << usr << "!!" << endl;
    }

    fclose(file);
}

void ExecuteLogout(Sesion *currentUser, bool *activeSession){
    if(!*activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    *activeSession = false;
    Sesion s;
    cout << "GOOD BYE " << currentUser->user.name << "!!" << endl;
    currentUser = &s;
}

int countGroups(string c){
    vector<string> content = getLines(c);
    int counter = 0;
    for(int i = 0; i < content.size(); i++){
        if(!isUser(content.at(i))){
            counter++;
        }
    }

    return counter;
}

bool existGroup(string name, string c){
    vector<string> content = getLines(c);
    for(int i = 0; i < content.size(); i++){
        if(!isUser(content.at(i))){
            string nameGroup,  aux;
            int counter = 0;
            for(int j = 0; j < content.at(i).length(); j++){
                if(content.at(i)[j] == ',' || j == content.at(i).length()-1){
                    if(counter == 2){
                            aux += content.at(i)[j];
                            nameGroup = aux;
                    }
                    aux = "";
                    counter++;
                    continue;
                }
                aux += content.at(i)[j];
            }

            if(nameGroup == name && content.at(i)[0] != '0'){
                return true;
            }
        }
    }

    return false;
}

int getGroupId(string name, string c){
    vector<string> content = getLines(c);
    for(int i = 0; i < content.size(); i++){
        if(!isUser(content.at(i))){
            string nameGroup,  aux;
            int counter = 0, id;
            for(int j = 0; j < content.at(i).length(); j++){
                if(content.at(i)[j] == ',' || j == content.at(i).length()-1){
                    if(counter == 2){
                        aux += content.at(i)[j];
                        nameGroup = aux;
                    }else if (counter == 0){
                        id = stoi(aux);
                    }
                    aux = "";
                    counter++;
                    continue;
                }
                aux += content.at(i)[j];
            }

            if(nameGroup == name && content.at(i)[0] != '0'){
                return id;
            }
        }
    }

    return -1;
}

int countUsers(string c){
    vector<string> content = getLines(c);
    int counter = 0;
    for(int i = 0; i < content.size(); i++){
        if(isUser(content.at(i))){
            counter++;
        }
    }

    return counter;
}

bool existUser(string name, string c){
    vector<string> content = getLines(c);
    for(int i = 0; i < content.size(); i++){
        if(isUser(content.at(i))){
            string nameGroup,  aux;
            int counter = 0;
            for(int j = 0; j < content.at(i).length(); j++){
                if(content.at(i)[j] == ','){
                    if(counter == 3){
                        nameGroup = aux;
                    }
                    aux = "";
                    counter++;
                    continue;
                }
                aux += content.at(i)[j];
            }

            if(nameGroup == name && content.at(i)[0] != '0'){
                return true;
            }
        }
    }

    return false;
}

string separateContent(string *content){
    string res, aux;
    for(int i = 0; i < content->length(); i++){
        if(i<64){
            res += (*content)[i];
            continue;
        }

        aux += (*content)[i];
    }

    *content = aux;
    return res;
}
//aqui
int getFreeBlock(SuperBlock sp, FILE *file){
    fseek(file, sp.s_bm_block_start, SEEK_SET);
    char c;
    int counter = 0;
    for(int i = sp.s_bm_block_start; i < sp.s_inode_start; i++){
        fread(&c, 1, 1, file);
        if(c == '0'){
            c = '1';
            fseek(file, i, SEEK_SET);
            fwrite(&c, sizeof(char), 1, file);
            return counter;
        }
        counter++;
    }
    return -1;
}

int getFreeInode(SuperBlock sp, FILE *file){
    fseek(file, sp.s_bm_inode_start, SEEK_SET);
    char c;
    int counter = 0;
    for(int i = sp.s_bm_inode_start; i < sp.s_inodes_count+sp.s_bm_inode_start; i++){
        fread(&c, 1, 1, file);
        if(c == '0'){
            c = '1';
            fseek(file, i, SEEK_SET);
            fwrite(&c, 1, 1, file);
            return counter;
        }
        counter++;
    }
    return -1;
}

//-1 = error, -2 = ya esta escrito
int writeInContentBlock(string content, int pointer, FILE *file, SuperBlock sp, int *createdBlocks){
    int p = pointer;
    bool newBlock = false;
    if(pointer == -1){
        p = getFreeBlock(sp, file);
        if(p == -1){
            cout << "$Error: no blocks available" << endl;
            return -1;
        }
        (*createdBlocks)++;
        newBlock = true;
    }

    FileBlock fb;
    strcpy(fb.b_content, content.c_str());
    fseek(file, sp.s_block_start+(p * 64), SEEK_SET);
    fwrite(&fb, sizeof(FileBlock), 1, file);

    if(newBlock){
        return p;
    }
    return -2;
}

void createPointerBlock(PointerBlock *pb){
    for(int i = 0; i < 16; i++){
        pb->b_pointers[i] = -1;
    }
}

int writeInPointerBlock(string *content, SuperBlock sp, FILE *file, int dim, int pointer, int *createdBlocks){
    PointerBlock pb;
    bool newPointer = false;
    if(pointer == -1){
        pointer = getFreeBlock(sp, file);
        if(pointer == -1){
            cout << "$Error: no blocks available" << endl;
            return -1;
        }

        newPointer = true;
        (*createdBlocks)++;
        fseek(file, sp.s_block_start+(pointer*64), SEEK_SET);
        createPointerBlock(&pb);
        fwrite(&pb, 64, 1, file);
    }
    fseek(file, sp.s_block_start+(pointer*64), SEEK_SET);
    fread(&pb, sizeof(PointerBlock), 1, file);

    if(dim == 1){
        for(int i = 0; i < 16; i++) {
            if(*content == ""){
                if(newPointer){
                    return pointer;
                }
                return -2;
            }
            int r = writeInContentBlock(separateContent(content), pb.b_pointers[i], file, sp, createdBlocks);
            if(r == -1){
                return -1;
            }else if(r == -2){
                continue;
            }

            pb.b_pointers[i] = r;

            fseek(file, sp.s_block_start+(pointer*64), SEEK_SET);
            fwrite(&pb, sizeof(PointerBlock), 1, file);
        }
        if(newPointer){
            return pointer;
        }
        return -2;
    }

    for(int i = 0; i < 16; i++) {
        if(*content == ""){
            if(newPointer){
                return pointer;
            }
            return -2;
        }
        int r = writeInPointerBlock(content, sp, file, dim-1, pb.b_pointers[i], createdBlocks);
        if(r == -1){
            return -1;
        }else if(r == -2){
            continue;
        }

        pb.b_pointers[i] = r;

        fseek(file, sp.s_block_start+(pointer*64), SEEK_SET);
        fwrite(&pb, sizeof(PointerBlock), 1, file);
    }
    if(newPointer){
        return pointer;
    }
    return -2;
}

bool writeInFile(SuperBlock sp, Inode *f, string content, FILE *file, int pointerInode, int *createdBlocks){
    int size = content.length();
    for(int i = 0; i < 15; i++){
        if(content == ""){
            return true;
        }
        int response;

        if(i < 12) {
            response = writeInContentBlock(separateContent(&content), f->i_block[i], file, sp, createdBlocks);
        }else{
            response = writeInPointerBlock(&content, sp, file, i-11, f->i_block[i], createdBlocks);
        }

        if(response == -1){
            return false;
        }else if(response == -2){
            continue;
        }

        f->i_block[i] = response;
        strcpy(f->i_mtime, currentDateTime().c_str());

        fseek(file, sp.s_inode_start+(pointerInode* sizeof(Inode)), SEEK_SET);
        fwrite(&f, sizeof(Inode), 1, file);
    }
    return true;
}

void ExecuteMkGrp(string name, Sesion *currentUser, bool *activeSession){
    if(name.length() > 10){
        cout << "$Error: the name is too long" << endl;
        return;
    }
    if(!*activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    if(string(currentUser->user.name) != "root"){
        cout << "$Error: you do not have permission to use this command" << endl;
        return;
    }

    MountedPartition *mountedPartition;
    mountedPartition = &(currentUser->mountedPartition);
    FILE *file = fopen(mountedPartition->path.c_str(), "rb+");

    SuperBlock sp;
    int start;
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
    }else{
        start = mountedPartition->par.part_start;
    }

    fseek(file, start, SEEK_SET);
    fread(&sp, sizeof(SuperBlock), 1, file);

    Inode root;
    fseek(file, sp.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);

    int pointerOfFile;

    root = searchFile(file, root, splitPath("users.txt"), sp.s_inode_start, sp.s_block_start, &pointerOfFile);
    if(root.i_type == 'n'){
        fclose(file);
        cout << "$Error: users.txt does not exist" << endl;
        return;
    }

    string content = readFile(file, root, sp.s_inode_start, sp.s_block_start);

    if(content.empty()){
        fclose(file);
        return;
    }

    if(existGroup(name, content)){
        cout << "$Error: the group already exist" << endl;
        fclose(file);
        return;
    }

    int id = countGroups(content)+1;
    string newGoup = to_string(id)+",G,"+name+"\n";

    content += newGoup;
    int createdBlocks = 0, newSize = content.length();
    if(writeInFile(sp, &root, content, file, pointerOfFile, &createdBlocks)){
        sp.s_free_blocks_counts-=createdBlocks;
        fseek(file, start, SEEK_SET);
        fwrite(&sp, sizeof(SuperBlock), 1, file);

        root.i_s = newSize;
        strcpy(root.i_mtime, currentDateTime().c_str());
        fseek(file, sp.s_inode_start+(sizeof(Inode)*pointerOfFile), SEEK_SET);
        fwrite(&root, sizeof(Inode), 1, file);
        cout << "GROUP CREATED SUCCESFULLY" << endl;
    }

    fclose(file);
}

void ExecuteMkusr(string name, string pass, string group, Sesion *currentUser, bool *activeSession){
    if(name.length() > 10){
        cout << "$Error: the user is too long" << endl;
        return;
    }
    if(name.length() > 10){
        cout << "$Error: the password is too long" << endl;
        return;
    }
    if(!*activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    if(string(currentUser->user.name) != "root"){
        cout << "$Error: you do not have permission to use this command" << endl;
        return;
    }

    MountedPartition *mountedPartition;
    mountedPartition = &(currentUser->mountedPartition);
    FILE *file = fopen(mountedPartition->path.c_str(), "rb+");

    SuperBlock sp;
    int start;
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
    }else{
        start = mountedPartition->par.part_start;
    }

    fseek(file, start, SEEK_SET);
    fread(&sp, sizeof(SuperBlock), 1, file);

    Inode root;
    fseek(file, sp.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);

    int pointerOfFile;
    root = searchFile(file, root, splitPath("users.txt"), sp.s_inode_start, sp.s_block_start, &pointerOfFile);
    if(root.i_type == 'n'){
        fclose(file);
        cout << "$Error: users.txt does not exist" << endl;
        return;
    }

    string content = readFile(file, root, sp.s_inode_start, sp.s_block_start);

    if(content.empty()){
        fclose(file);
        return;
    }

    if(existUser(name, content)){
        cout << "$Error: the user already exist" << endl;
        fclose(file);
        return;
    }

    if(!existGroup(group, content)){
        cout << "$Error: the group does not exist" << endl;
        fclose(file);
        return;
    }

    int id = countUsers(content)+1;
    string newUser = to_string(id)+",U,"+group+","+name+","+pass+"\n";

    content += newUser;
    int newSize = content.length();
    int createdBlocks = 0;
    if(writeInFile(sp, &root, content, file, pointerOfFile, &createdBlocks)){
        sp.s_free_blocks_counts-=createdBlocks;
        fseek(file, start, SEEK_SET);
        fwrite(&sp, sizeof(SuperBlock), 1, file);

        root.i_s = newSize;
        strcpy(root.i_mtime, currentDateTime().c_str());
        fseek(file, sp.s_inode_start+(sizeof(Inode)*pointerOfFile), SEEK_SET);
        fwrite(&root, sizeof(Inode), 1, file);
        cout << "USER CREATED SUCCESFULLY" << endl;
    }

    fclose(file);
}

void ExecuteRmgrp(string name, Sesion *currentUser, bool *activeSession){
    if(name == "root"){
        cout << "$Error: you can not delete the root group" << endl;
        return;
    }
    if(!*activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    if(string(currentUser->user.name) != "root"){
        cout << "$Error: you do not have permission to use this command" << endl;
        return;
    }

    MountedPartition *mountedPartition;
    mountedPartition = &(currentUser->mountedPartition);
    FILE *file = fopen(mountedPartition->path.c_str(), "rb+");

    SuperBlock sp;
    int start;
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
    }else{
        start = mountedPartition->par.part_start;
    }

    fseek(file, start, SEEK_SET);
    fread(&sp, sizeof(SuperBlock), 1, file);

    Inode root;
    fseek(file, sp.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);

    int pointerOfFile;
    root = searchFile(file, root, splitPath("users.txt"), sp.s_inode_start, sp.s_block_start, &pointerOfFile);
    if(root.i_type == 'n'){
        fclose(file);
        cout << "$Error: users.txt does not exist" << endl;
        return;
    }

    string content = readFile(file, root, sp.s_inode_start, sp.s_block_start);

    if(content.empty()){
        fclose(file);
        return;
    }

    if(!existGroup(name, content)){
        cout << "$Error: the group does not exist" << endl;
        fclose(file);
        return;
    }

    vector<string> c = getLines(content);

    for(int i = 0; i < c.size(); i++){
        if(!isUser(c.at(i))){
            int id, counter = 0;
            string group, aux = "";
            for(int j = 0; j < c.at(i).length(); j++){
                if(c.at(i)[j] == ',' || j == c.at(i).length()-1){
                    switch (counter) {
                        case 0:
                            id = stoi(aux);
                            break;
                        case 2:
                            aux += c.at(i)[j];
                            group = aux;
                            break;
                    }
                    aux = "";
                    counter++;
                    continue;
                }
                aux += c.at(i)[j];
            }

            if(name == group){
               if(id == 0){
                   cout << "$Error: group does not exist" << endl;
                   fclose(file);
                   return;
               }
               c.at(i) = "0,G,"+name;
               break;
            }
        }
    }

    string finalContent;
    for(int i = 0; i < c.size(); i++){
        finalContent += c.at(i)+"\n";
    }

    int newSize = finalContent.length();
    int createdBlocks = 0;
    if(writeInFile(sp, &root, finalContent, file, pointerOfFile, &createdBlocks)){
        sp.s_free_blocks_counts-=createdBlocks;
        fseek(file, start, SEEK_SET);
        fwrite(&sp, sizeof(SuperBlock), 1, file);

        root.i_s = newSize;
        strcpy(root.i_mtime, currentDateTime().c_str());
        fseek(file, sp.s_inode_start+(sizeof(Inode)*pointerOfFile), SEEK_SET);
        fwrite(&root, sizeof(Inode), 1, file);
        cout << "GROUP DELETED SUCCESFULLY" << endl;
    }

    fclose(file);
}

void ExecuteRmusr(string name, Sesion *currentUser, bool *activeSession){
    if(name == "root"){
        cout << "$Error: you can not delete the root user" << endl;
        return;
    }
    if(!*activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    if(string(currentUser->user.name) != "root"){
        cout << "$Error: you do not have permission to use this command" << endl;
        return;
    }

    MountedPartition *mountedPartition;
    mountedPartition = &(currentUser->mountedPartition);
    FILE *file = fopen(mountedPartition->path.c_str(), "rb+");

    SuperBlock sp;
    int start;
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
    }else{
        start = mountedPartition->par.part_start;
    }

    fseek(file, start, SEEK_SET);
    fread(&sp, sizeof(SuperBlock), 1, file);

    Inode root;
    fseek(file, sp.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);

    int pointerOfFile;
    root = searchFile(file, root, splitPath("users.txt"), sp.s_inode_start, sp.s_block_start, &pointerOfFile);
    if(root.i_type == 'n'){
        fclose(file);
        cout << "$Error: users.txt does not exist" << endl;
        return;
    }

    string content = readFile(file, root, sp.s_inode_start, sp.s_block_start);

    if(content.empty()){
        fclose(file);
        return;
    }

    if(!existUser(name, content)){
        cout << "$Error: the user does not exist" << endl;
        fclose(file);
        return;
    }

    vector<string> c = getLines(content);

    for(int i = 0; i < c.size(); i++){
        if(isUser(c.at(i))){
            int id, counter = 0;
            string group, user, pass, aux = "";
            for(int j = 0; j < c.at(i).length(); j++){
                if(c.at(i)[j] == ',' || j == c.at(i).length()-1){
                    switch (counter) {
                        case 0:
                            id = stoi(aux);
                            break;
                        case 2:
                            group = aux;
                            break;
                        case 3:
                            user = aux;
                            break;
                        case 4:
                            aux += c.at(i)[j];
                            pass = aux;
                            break;
                    }
                    aux = "";
                    counter++;
                    continue;
                }
                aux += c.at(i)[j];
            }

            if(name == user){
                if(id == 0){
                    cout << "$Error: user does not exist" << endl;
                    fclose(file);
                    return;
                }
                c.at(i) = "0,U,"+group+","+name+","+pass;
            }
        }
    }

    string finalContent;
    for(int i = 0; i < c.size(); i++){
        finalContent += c.at(i)+"\n";
    }

    int newSize = finalContent.length();
    int createdBlocks = 0;
    if(writeInFile(sp, &root, finalContent, file, pointerOfFile, &createdBlocks)){
        sp.s_free_blocks_counts-=createdBlocks;
        fseek(file, start, SEEK_SET);
        fwrite(&sp, sizeof(SuperBlock), 1, file);

        root.i_s = newSize;
        strcpy(root.i_mtime, currentDateTime().c_str());
        fseek(file, sp.s_inode_start+(sizeof(Inode)*pointerOfFile), SEEK_SET);
        fwrite(&root, sizeof(Inode), 1, file);
        cout << "USER DELETED SUCCESFULLY" << endl;
    }

    fclose(file);
}

void ExecuteChmod(int ugo, string path, bool r, Sesion currentUser, bool activeSession){
    if(!activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    if(currentUser.user.name != "root"){
        cout << "$Error: you do not have permission to use this command" << endl;
        return;
    }

    if(ugo < 100 || ugo >777){
        cout << "$Error: the value for ugo is not allowed" << endl;
        return;
    }

    int start;
    if(currentUser.mountedPartition.isLogic){
        start = currentUser.mountedPartition.logicPar.part_start;
    }else{
        start = currentUser.mountedPartition.par.part_start;
    }

    SuperBlock sb;
    FILE *file = fopen(currentUser.mountedPartition.path.c_str(), "rb+");
    fseek(file, start, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, file);

    Inode root, aux;
    fseek(file, sb.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);
    int pointerOfFile = 0;
    aux = searchFile(file, root, splitPath(path), sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    if(r){
        if(aux.i_type == 'n'){
            fclose(file);
            cout << "$Error: path does not exist" << endl;
            return;
        }

        changeUGO(file, root, splitPath(path), sb.s_inode_start, sb.s_block_start, ugo);
        cout << "SUCCESSFULLY MODIFIED PERMISSIONS"<< endl;
        fclose(file);
        return;
    }

    fseek(file, sb.s_inode_start+(sizeof(Inode)*pointerOfFile), SEEK_SET);
    aux.i_perm = ugo;
    strcpy(aux.i_mtime, currentDateTime().c_str());

    fwrite(&aux, sizeof(Inode), 1, file);
    cout << "SUCCESSFULLY MODIFIED PERMISSIONS"<< endl;
    fclose(file);
    return;
}

void permission(bool *read, bool *write, bool *exec, int num){
    switch (num) {
        case 0:
            *read = 0;
            *write = 0;
            *exec = 0;
            break;
        case 1:
            *read = 0;
            *write = 0;
            *exec = 1;
            break;
        case 2:
            *read = 0;
            *write = 1;
            *exec = 0;
            break;
        case 3:
            *read = 0;
            *write = 1;
            *exec = 1;
            break;
        case 4:
            *read = 1;
            *write = 0;
            *exec = 0;
            break;
        case 5:
            *read = 1;
            *write = 0;
            *exec = 1;
            break;
        case 6:
            *read = 1;
            *write = 1;
            *exec = 0;
            break;
        case 7:
            *read = 1;
            *write = 1;
            *exec = 1;
            break;
    }
}

bool getPermission(Inode inode, int userId, int groupId, int ugo, bool read, bool write, bool execution){
    int u, g, o;
    bool r = false, w = false, x = false;
    u = floor(ugo/100);
    g = floor((ugo%100)/10);
    o = ugo%10;

    if(inode.i_uid == userId){
        permission(&r,&w,&x,u);
    }else if(inode.i_gid == groupId){
        permission(&r,&w,&x,g);
    }else{
        permission(&r,&w,&x,o);
    }

    if(read){
        return r;
    }

    if(write){
        return w;
    }

    return o;
}

/*bool getPermissionOfDirectories(vector<string> path, Inode root, bool read, bool write, bool exec, FILE *file,
                                int istart, int bstart){
    Inode aux;
    vector<string> auxPath;
    int p = 0;
    for(int i = 0; i < path.size(); i++){
        auxPath.push_back(path.at(i));
        aux = searchFile(file, root, auxPath, istart, bstart, &p);

    }

    return false;
}*/

void newDirBlock(DirBlock *n){
    for(int i = 0; i < 4; i++){
        n->b_content[i].b_inodo = -1;
        strcpy(n->b_content[i].b_name, "");
    }
}

int writeInode(int type, User user, FILE *file, int istart, int bstart, Sesion *currentUser, int father, int *createdBlocks, int *createdInodes, int perm){
    MountedPartition *mountedPartition;
    mountedPartition = &(currentUser->mountedPartition);

    SuperBlock sp;
    int start;
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
    }else{
        start = mountedPartition->par.part_start;
    }

    fseek(file, start, SEEK_SET);
    fread(&sp, sizeof(SuperBlock), 1, file);

    Inode root;
    fseek(file, istart, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);
    int pointerOfFile;

    root = searchFile(file, root, splitPath("users.txt"), istart, bstart, &pointerOfFile);
    if(root.i_type == 'n'){
        fclose(file);
        cout << "$Error: users.txt does not exist==" << endl;
        return -1;
    }

    string content = readFile(file, root, sp.s_inode_start, sp.s_block_start);

    if(content.empty()){
        fclose(file);
        return -1;
    }

    int groupId = getGroupId(string(user.group), content);

    Inode newInode;
    int newInodePointer = getFreeInode(sp, file);
    if(newInodePointer == -1){
        return -1;
    }

    for(int i = 0; i < 15; i++){
        newInode.i_block[i] = -1;
    }
    strcpy(newInode.i_ctime, currentDateTime().c_str());
    newInode.i_uid = user.id;
    newInode.i_gid = groupId;
    newInode.i_perm = perm;
    newInode.i_type = '1';
    newInode.i_s = 0;
    if(type == 0){
        newInode.i_type = '0';
        int newBlock = getFreeBlock(sp, file);
        if(newBlock == -1){
            return -1;
        }

        (*createdBlocks)++;
        DirBlock db;
        newDirBlock(&db);
        db.b_content[0].b_inodo = newInodePointer;
        strcpy(db.b_content[0].b_name, ".");
        db.b_content[1].b_inodo = father;
        strcpy(db.b_content[1].b_name, "..");

        fseek(file, bstart+(64*newBlock), SEEK_SET);
        fwrite(&db, 64, 1, file);
        newInode.i_block[0] = newBlock;
    }

    fseek(file, istart+(sizeof(Inode)*newInodePointer), SEEK_SET);
    fwrite(&newInode, sizeof(Inode), 1, file);
    (*createdInodes)++;
    return newInodePointer;
}

int writeInodeInPointerBlock(int dim, int pointer, FILE *file, Inode *father, int istart, int bstart, User user,
                             Sesion *currentUser, string name, int type, SuperBlock sp, int fatherPointer, int *createdBlocks, int *createdInodes, int perm){
    fseek(file, bstart+(64*pointer), SEEK_SET);
    PointerBlock pb;
    fread(&pb, sizeof(PointerBlock), 1, file);

    if(dim == 1){
        for(int i = 0; i < 16; i++){
            if(pb.b_pointers[i] == -1){
                DirBlock db;
                newDirBlock(&db);

                int newBlock = getFreeBlock(sp, file);

                if(newBlock == -1){
                    return -1;
                }

                int newInode = writeInode(type, user, file, istart, bstart, currentUser, fatherPointer, createdBlocks, createdInodes, perm);
                if(newInode == -1){
                    return -1;
                }

                db.b_content[0].b_inodo = newInode;
                strcpy(db.b_content[0].b_name, name.c_str());

                pb.b_pointers[i] = newBlock;

                fseek(file, bstart+(64*pointer), SEEK_SET);
                fwrite(&pb, sizeof(PointerBlock), 1, file);

                fseek(file, bstart+(64*newBlock), SEEK_SET);
                fwrite(&db, sizeof(DirBlock), 1, file);

                return -2;
            }else{
                DirBlock db;
                fseek(file, bstart+(64*pb.b_pointers[i]), SEEK_SET);
                fread(&db, sizeof(DirBlock), 1, file);

                for(int j = 0; j < 4; j++){
                    if(string(db.b_content[j].b_name) == name){
                        cout << "$Error: "<<name<<"already exist"<<endl;
                        return -1;
                    }
                    if(db.b_content[j].b_inodo == -1){
                        int newInode = writeInode(type, user, file, istart, bstart, currentUser, fatherPointer, createdBlocks, createdInodes, perm);
                        if(newInode == -1){
                            return -1;
                        }
                        strcpy(db.b_content[j].b_name, name.c_str());
                        db.b_content[j].b_inodo = newInode;

                        fseek(file, bstart+(64*pb.b_pointers[i]), SEEK_SET);
                        fwrite(&db, sizeof(DirBlock), 1, file);
                        return -2;
                    }
                }
            }
        }
    }else{
        for(int i = 0; i < 16; i++){
            if(pb.b_pointers[i] == -1){
                PointerBlock npb;
                createPointerBlock(&npb);

                int newBlock = getFreeBlock(sp, file);
                if(newBlock == -1){
                    return -1;
                }
                fseek(file, bstart+(64*newBlock), SEEK_SET);
                fwrite(&npb, sizeof(PointerBlock), 1, file);

                (*createdBlocks)++;

                pb.b_pointers[i] = newBlock;
                fseek(file, bstart+(64*pointer), SEEK_SET);
                fwrite(&pb, sizeof(PointerBlock), 1, file);
            }
            int res = writeInodeInPointerBlock(dim-1, pb.b_pointers[i], file, father, istart, bstart, user, currentUser,
                                               name, type, sp, fatherPointer, createdBlocks, createdInodes, perm);

            if(res == -1 || res == -2){
                return res;
            }
        }
    }

    return -3;
}

int createDirectory(FILE *file, Inode *father, int istart, int bstart, User user, Sesion *currentUser, string name,
                     int type, SuperBlock sp, int *createdBlocks, int *createdInodes, int *p, int perm){
    fseek(file, bstart+(64*father->i_block[0]), SEEK_SET);
    DirBlock dbf;
    fread(&dbf, sizeof(DirBlock), 1, file);
    int fatherPointer = dbf.b_content[0].b_inodo;
    for(int i = 0; i < 15; i++){
        if(i < 12){
            if(father->i_block[i] == -1){
                int pointerOfNewDir = getFreeBlock(sp, file);
                if(pointerOfNewDir == -1){
                    return -1;
                }
                DirBlock newDir;
                newDirBlock(&newDir);

                int newInode = writeInode(type, user, file, istart, bstart, currentUser, fatherPointer, createdBlocks, createdInodes, perm);
                if(newInode == -1){
                    return -1;
                }

                newDir.b_content[0].b_inodo = newInode;
                strcpy(newDir.b_content[0].b_name, name.c_str());

                father->i_block[i] = pointerOfNewDir;

                //insertamos el nuevo dirblock
                fseek(file, bstart+(64*pointerOfNewDir), SEEK_SET);
                fwrite(&newDir, sizeof(DirBlock), 1, file);

                //insertamos al padre con el puntero escrito
                fseek(file, istart+(sizeof(Inode)*fatherPointer), SEEK_SET);
                //Inode auxFather = *father;
                fwrite(father, sizeof(Inode), 1, file);
                return -2;
            }else{
                DirBlock dirBlock;
                fseek(file, bstart+(father->i_block[i]*64), SEEK_SET);
                fread(&dirBlock, sizeof(DirBlock), 1, file);

                for(int j = 0; j < 4; j++){
                    if(dirBlock.b_content[j].b_name == name){
                        cout << "$Error: "<< name << " already exist"<<endl;
                        return -1;
                    }
                    if(dirBlock.b_content[j].b_inodo == -1){
                        int newInode = writeInode(type, user, file, istart, bstart, currentUser, fatherPointer, createdBlocks, createdInodes, perm);
                        if(newInode == -1){
                            return -1;
                        }
                        dirBlock.b_content[j].b_inodo = newInode;
                        strcpy(dirBlock.b_content[j].b_name, name.c_str());

                        fseek(file, bstart+(father->i_block[i]*64), SEEK_SET);
                        fwrite(&dirBlock, sizeof(DirBlock), 1, file);
                        return -2;
                    }
                }
            }
        }else{
            if(father->i_block[i] == -1){
                PointerBlock pb;
                createPointerBlock(&pb);
                int newBlock = getFreeBlock(sp, file);
                if(newBlock == -1){
                    return -1;
                }

                father->i_block[i] = newBlock;
                fseek(file, bstart+(64*newBlock), SEEK_SET);
                fwrite(&pb, sizeof(PointerBlock), 1, file);

                fseek(file, istart+(sizeof(Inode)*fatherPointer), SEEK_SET);
                fwrite(father, sizeof(Inode), 1, file);

                (*createdBlocks)++;
            }

            int res = writeInodeInPointerBlock(i-11, father->i_block[i], file, father, istart, bstart, user, currentUser,
                                               name, type, sp, fatherPointer, createdBlocks, createdInodes, perm);

            if(res == -1 || res == -2){
                return res;
            }
        }/*else{
            if(father->i_block[i] == -1){
                PointerBlock npb;
                createPointerBlock(&npb);
                int newP = getFreeBlock(sp, file);
                if(newP == -1){
                    return -1;
                }
                fseek(file, bstart+(64*newP), SEEK_SET);
                fwrite(&npb, sizeof(PointerBlock), 1, file);
                father->i_block[i] = newP;
                (*createdBlocks)++;
                //strcpy(father->i_mtime, currentDateTime().c_str());
                fseek(file, istart+(sizeof(Inode)*fatherPointer), SEEK_SET);
                cout << "*********  "<< istart+(sizeof(Inode)*fatherPointer) << endl;
                fwrite(&father, sizeof(Inode), 1, file);
                cout << "pointer of pointer block: "<<father->i_block[i] << endl;
                cout << "perm of father: "<<father->i_perm<<endl;/*
                int response = writeInodeInPointerBlock(i-11, newP, file, father, istart, bstart, user,
                                                        currentUser, name, type, sp, fatherPointer, createdBlocks, createdInodes);
                fseek(file, istart+(sizeof(Inode)*fatherPointer), SEEK_SET);
                fread(&father, sizeof(Inode), 1, file);
                cout << "perm of father: "<<father->i_perm<<endl;
                return -2;
            }
            int response = writeInodeInPointerBlock(i-11, father->i_block[i], file, father, istart, bstart, user,
                                                    currentUser, name, type, sp, fatherPointer, createdBlocks, createdInodes);
            if(response == -1 || response == -2){
                return response;
            }
        }*/
    }
}
/*void createDirectory(FILE *file, Inode *father, int istart, int bstart, User user, Sesion *currentUser, string name,
                     int type, SuperBlock sp, int *createdBlocks, int *createdInodes, int *p){
    fseek(file, bstart+(64*father->i_block[0]), SEEK_SET);
    DirBlock dbf;
    fread(&dbf, sizeof(DirBlock), 1, file);
    for(int i = 0; i < 15; i++){
        if(i < 12){
            if(father->i_block[i] != -1){
                fseek(file, bstart+(64*father->i_block[i]), SEEK_SET);
                DirBlock db;
                fread(&db, sizeof(DirBlock), 1, file);
                for(int j = 0; j < 4; j++){
                    if(string(db.b_content[j].b_name) == name){
                        cout << "$Error: "<< name << " already exist" << endl;
                        return;
                    }
                    if(db.b_content[j].b_inodo == -1){
                        //int newInode = writeInode(type, user, file, istart, bstart, currentUser, dbf.b_content[0].b_inodo, createdBlocks, createdInodes);
                        //if(newInode != -1) {
                            db.b_content[j].b_inodo = -2;//newInode;
                            strcpy(db.b_content[j].b_name, name.c_str());
                            fseek(file, bstart + (64 * father->i_block[i]), SEEK_SET);
                            fwrite(&db, 64, 1, file);

                            //fseek(file, istart+(sizeof(Inode)*newInode), SEEK_SET);
                            //fread(father, sizeof(Inode), 1, file);
                            //*p = newInode;
                            return;
                        //}
                    }
                }
            }else{
                int newDirB = getFreeBlock(sp, file);
                if(newDirB == -1){
                    return;
                }
                DirBlock db;
                newDirBlock(&db);
                fseek(file, bstart+(64*newDirB), SEEK_SET);
                fwrite(&db, sizeof(DirBlock), 1, file);

                //int newInode = writeInode(type, user, file, istart, bstart, currentUser, dbf.b_content[0].b_inodo, createdBlocks, createdInodes);
                //if(newInode != -1) {
                    db.b_content[0].b_inodo = -2;//newInode;
                    strcpy(db.b_content[0].b_name, name.c_str());
                    fseek(file, bstart + (64 * newDirB), SEEK_SET);
                    fwrite(&db, 64, 1, file);
                //}

                fseek(file, istart+(sizeof(Inode)*dbf.b_content[0].b_inodo), SEEK_SET);
                father->i_block[i] = newDirB;
                strcpy(father->i_mtime, currentDateTime().c_str());

                fwrite(&father, sizeof(Inode), 1, file);
                (*createdBlocks)++;

                //fseek(file, istart+(sizeof(Inode)*newInode), SEEK_SET);
                //fread(father, sizeof(Inode), 1, file);
                //*p = newInode;
                return;
                //i--;
                //continue;
            }
        }else{
            if(father->i_block[i] == -1){
                PointerBlock npb;
                createPointerBlock(&npb);
                int newP = getFreeBlock(sp, file);
                if(newP == -1){
                    return;
                }
                fseek(file, bstart+(64*newP), SEEK_SET);
                fwrite(&npb, 64, 1, file);
                father->i_block[i] = newP;
                (*createdBlocks)++;
                strcpy(father->i_mtime, currentDateTime().c_str());
                fseek(file, istart+(sizeof(Inode)*dbf.b_content[0].b_inodo), SEEK_SET);
                fwrite(&father, 64, 1, file);

                int response = writeInodeInPointerBlock(i-12, newP, file, father, istart, bstart, user,
                                                        currentUser, name, type, sp, dbf.b_content[0].b_inodo, createdBlocks, createdInodes);
                return;
            }
            int response = writeInodeInPointerBlock(i-12, father->i_block[i], file, father, istart, bstart, user,
                                                    currentUser, name, type, sp, dbf.b_content[0].b_inodo, createdBlocks, createdInodes);
            if(response == -1 || response == -2){
                return;
            }
        }
    }

    cout << "$Error: no space available" << endl;
}*/

bool createMultipleDirectories(vector<string> path, Inode father, FILE *file, Inode root, int istart, int bstart,
                               int userId, int groupId, Sesion currentUser, SuperBlock sp, int *createdBlocks, int *createdInodes, int *p, int perm){

    vector<string> auxPath;
    Inode response = root;
    for(int i = 0; i < path.size(); i++){
        auxPath.push_back(path.at(i));
        response = searchFile(file, root, auxPath, istart, bstart, p);
        if(response.i_type == 'n'){
            if (getPermission(father, userId, groupId, father.i_perm, 0, 1, 0)) {
                createDirectory(file, &father, istart, bstart, currentUser.user, &currentUser, path.at(i), 0, sp, createdBlocks, createdInodes, p, perm);
                fseek(file, sp.s_inode_start, SEEK_SET);
                fread(&root, sizeof(Inode), 1, file);
                father = searchFile(file, root, auxPath, istart, bstart, p);
            } else {
                cout << "$Error: you dont have permission to write" << endl;
                return false;
            }
        }else{
            father = response;
        }
    }

    return true;
}

void ExecMkfile(string path, bool r, int size, string contPath, Sesion currentUser, bool activeSession, int perm){
    if(!activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    string content;
    if(contPath != "") {
        if (!Exist(contPath)) {
            cout << "$Error: the file for cont does not exist" << endl;
            return;
        }

        fstream file;
        file.open(contPath, ios::in);

        if(file.is_open()){
            string line;
            while(getline(file, line)){
                content += line+"\n";
            }
        }
    }else if(size != 0){
        for(int i = 0; i < size; i++){
            content += to_string(i%10);
        }
    }

    int start;
    if(currentUser.mountedPartition.isLogic){
        start = currentUser.mountedPartition.logicPar.part_start;
    }else{
        start = currentUser.mountedPartition.par.part_start;
    }

    SuperBlock sb;
    FILE *file = fopen(currentUser.mountedPartition.path.c_str(), "rb+");
    fseek(file, start, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, file);

    Inode root, aux;
    fseek(file, sb.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);
    int pointerOfFile = 0;
    aux = searchFile(file, root, splitPath("users.txt"),sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    string c = readFile(file, aux, sb.s_inode_start, sb.s_block_start);
    vector<string> p, paux = splitPath(path);

    for(int i = 0; i < paux.size(); i++){
        if(i < paux.size()-1){
            p.push_back(paux.at(i));
        }
    }
    if(p.size() == 0){
        aux = root;
    }else {
        aux = searchFile(file, root, p, sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    }

    int createdBlocks = 0, createdInodes = 0;
    if(r){
        if(aux.i_type == 'n'){
            if(!createMultipleDirectories(p, root, file, root, sb.s_inode_start, sb.s_block_start, currentUser.user.id,
                                         getGroupId(currentUser.user.group, c), currentUser, sb, &createdBlocks, &createdInodes, &pointerOfFile, perm)){
                fclose(file);
                return;
            }
            fseek(file, sb.s_inode_start, SEEK_SET);
            fread(&root, sizeof(Inode), 1, file);
            aux = searchFile(file, root, p, sb.s_inode_start, sb.s_block_start, &pointerOfFile);
        }
    }

    if(aux.i_type == 'n'){
        cout << "$Error: The directory does not exist" << endl;
    }else{
        if(getPermission(aux, currentUser.user.id, getGroupId(currentUser.user.group, c),
                         aux.i_perm, 0, 1 ,0)){
            fseek(file, sb.s_inode_start, SEEK_SET);
            fread(&root, sizeof(Inode), 1, file);
            if(createDirectory(file, &aux, sb.s_inode_start, sb.s_block_start, currentUser.user, &currentUser,
                            paux.at(paux.size()-1), 1, sb, &createdBlocks, &createdInodes, &pointerOfFile, perm) == -1){
                fclose(file);
                return;
            }
            fseek(file, sb.s_inode_start, SEEK_SET);
            fread(&root, sizeof(Inode), 1, file);
            aux = searchFile(file, root, splitPath(path), sb.s_inode_start, sb.s_block_start, &pointerOfFile);
            if(writeInFile(sb, &aux, content, file, pointerOfFile, &createdBlocks)){
                aux.i_s = content.length();
                fseek(file, sb.s_inode_start+(sizeof(Inode)*pointerOfFile), SEEK_SET);
                fwrite(&aux, sizeof(Inode), 1, file);
            }
            fseek(file, sb.s_inode_start, SEEK_SET);
            fread(&root, sizeof(Inode), 1, file);

            sb.s_free_blocks_counts -= createdBlocks;
            sb.s_free_inodes_count -= createdInodes;

            fseek(file, start, SEEK_SET);
            fwrite(&sb, sizeof(SuperBlock), 1, file);
            cout << "FILE CREATED SUCCESFULLY"<< endl;
            if(sb.s_filesystem_type == 3){
                writeJournal(start, sb, "mkfile", 1, path, content, currentUser.user.id, perm, file);
            }
        }else{
            cout << "$Error: you dont have permission to write" << endl;
        }
    }

    fclose(file);
}

void ExecuteMkdir(string path, bool r, Sesion currentUser, bool activeSession, int perm){
    if(!activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    int start;
    if(currentUser.mountedPartition.isLogic){
        start = currentUser.mountedPartition.logicPar.part_start;
    }else{
        start = currentUser.mountedPartition.par.part_start;
    }

    SuperBlock sb;
    FILE *file = fopen(currentUser.mountedPartition.path.c_str(), "rb+");
    fseek(file, start, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, file);

    Inode root, aux;
    fseek(file, sb.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);
    int pointerOfFile = 0;
    aux = searchFile(file, root, splitPath("users.txt"),sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    string c = readFile(file, aux, sb.s_inode_start, sb.s_block_start);
    vector<string> p, paux = splitPath(path);

    for(int i = 0; i < paux.size(); i++){
        if(i < paux.size()-1){
            p.push_back(paux.at(i));
        }
    }
    if(p.size() == 0){
        aux = root;
    }else {
        aux = searchFile(file, root, p, sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    }

    int createdBlocks = 0, createdInodes = 0;
    if(r){
        if(aux.i_type == 'n'){
            if(!createMultipleDirectories(p, root, file, root, sb.s_inode_start, sb.s_block_start, currentUser.user.id,
                                          getGroupId(currentUser.user.group, c), currentUser, sb, &createdBlocks, &createdInodes, &pointerOfFile, perm)){
                fclose(file);
                return;
            }
            fseek(file, sb.s_inode_start, SEEK_SET);
            fread(&root, sizeof(Inode), 1, file);
            aux = searchFile(file, root, p, sb.s_inode_start, sb.s_block_start, &pointerOfFile);
        }
    }

    if(aux.i_type == 'n'){
        cout << "$Error: The directory does not exist" << endl;
    }else{
        if(getPermission(aux, currentUser.user.id, getGroupId(currentUser.user.group, c),
                         aux.i_perm, 0, 1 ,0)){
            if(createDirectory(file, &aux, sb.s_inode_start, sb.s_block_start, currentUser.user, &currentUser,
                               paux.at(paux.size()-1), 0, sb, &createdBlocks, &createdInodes, &pointerOfFile, perm) == -1){
                fclose(file);
                return;
            }
            fseek(file, sb.s_inode_start, SEEK_SET);
            fread(&root, sizeof(Inode), 1, file);

            sb.s_free_blocks_counts -= createdBlocks;
            sb.s_free_inodes_count -= createdInodes;

            fseek(file, start, SEEK_SET);
            fwrite(&sb, sizeof(SuperBlock), 1, file);
            cout << "DIRECTORY CREATED SUCCESFULLY"<< endl;
            if(sb.s_filesystem_type == 3){
                writeJournal(start, sb, "mkdir", 0, path, "", currentUser.user.id, perm, file);
            }
        }else{
            cout << "$Error: you dont have permission to write" << endl;
        }
    }

    fclose(file);
}

void ExecuteCat(vector<string> files, Sesion currentUser, bool activeSession){
    if(!activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    int start;
    if(currentUser.mountedPartition.isLogic){
        start = currentUser.mountedPartition.logicPar.part_start;
    }else{
        start = currentUser.mountedPartition.par.part_start;
    }

    SuperBlock sb;
    FILE *file = fopen(currentUser.mountedPartition.path.c_str(), "rb+");
    fseek(file, start, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, file);

    Inode root, aux;
    fseek(file, sb.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);
    int pointerOfFile = 0;
    aux = searchFile(file, root, splitPath("users.txt"),sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    string c = readFile(file, aux, sb.s_inode_start, sb.s_block_start);

    string finalContent;
    for(int i = 0; i < files.size(); i++){
        aux = searchFile(file, root, splitPath(files.at(i)), sb.s_inode_start, sb.s_block_start, &pointerOfFile);
        if(aux.i_type == 'n'){
            cout << "$Error: file does not exist" << endl;
            fclose(file);
            return;
        }

        if(getPermission(aux, currentUser.user.id, getGroupId(currentUser.user.group, c),
                         aux.i_perm, 1, 0,0)){
            finalContent += readFile(file, aux, sb.s_inode_start, sb.s_block_start) + "\n";
        }else{
            cout << "$Error: You do not have permission to read " << files.at(i) << endl;
            fclose(file);
            return;
        }
    }
    cout << "=======================================================================" << endl;
    cout << finalContent << endl;
    cout << "=======================================================================" << endl;
    fclose(file);
}

void ExecuteEdit(string path, string contPath, Sesion currentUser, bool activeSession){
    if(!activeSession){
        cout << "$Error: there is no active session" << endl;
        return;
    }

    int start;
    if(currentUser.mountedPartition.isLogic){
        start = currentUser.mountedPartition.logicPar.part_start;
    }else{
        start = currentUser.mountedPartition.par.part_start;
    }

    SuperBlock sb;
    FILE *file = fopen(currentUser.mountedPartition.path.c_str(), "rb+");
    fseek(file, start, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, file);

    Inode root, aux;
    fseek(file, sb.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);
    int pointerOfFile = 0;
    aux = searchFile(file, root, splitPath("users.txt"),sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    string c = readFile(file, aux, sb.s_inode_start, sb.s_block_start);

    string content;
    if(contPath != "") {
        if (!Exist(contPath)) {
            cout << "$Error: the file for cont does not exist" << endl;
            fclose(file);
            return;
        }

        fstream file;
        file.open(contPath, ios::in);

        if(file.is_open()){
            string line;
            while(getline(file, line)){
                content += line+"\n";
            }
        }
    }

    aux = searchFile(file, root, splitPath(path), sb.s_inode_start, sb.s_block_start, &pointerOfFile);
    if(aux.i_type == 'n'){
        fclose(file);
        cout << "$Error: "<< path<<" does not exist"<<endl;
        return;
    }

    int createdBlocks = 0;
    if(getPermission(aux, currentUser.user.id, getGroupId(currentUser.user.group, c),aux.i_perm, 0, 1, 0)){
        if(writeInFile(sb, &aux, content, file, pointerOfFile, &createdBlocks)){
            aux.i_s = content.length();
            fseek(file, sb.s_inode_start+(sizeof(Inode)*pointerOfFile), SEEK_SET);
            fwrite(&aux, sizeof(Inode), 1, file);
        }else{
            return;
        }
        fseek(file, sb.s_inode_start, SEEK_SET);
        fread(&root, sizeof(Inode), 1, file);

        sb.s_free_blocks_counts -= createdBlocks;

        fseek(file, start, SEEK_SET);
        fwrite(&sb, sizeof(SuperBlock), 1, file);
        cout << "EDITION COMPLETED"<< endl;
    }else{
        cout << "$Error: you do not have permission to edit"<<endl;
    }

    fclose(file);
}