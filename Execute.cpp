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
using namespace std;

string ToLower(string data);

bool Exist(string path){
    FILE *archivo;

    if((archivo = fopen(path.c_str(), "r"))){
        fclose(archivo);
        return true;
    }

    return false;
}

const string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf,sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

void ExecuteMkdisk(int size, string unit, char fit, string path){
    if(!(Exist(path))){
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
/*
 *
 * FDISK
 *
 */

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

            /*for(int i = 0; i<partitions.size();i++){
                cout << partitions.at(i).part_start<<" - "<<partitions.at(i).part_name <<" - "<<partitions.at(i).part_next<<endl;
            }*/

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
            cout << "===$Error: the partition doesn't exist"<<endl;
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

            //pointer = ebr.part_start + ebr.part_s;
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