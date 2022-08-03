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
        char buff[1024];
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

        for(i = 0; i<1024; i++){
            buff[i] = '\0';
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
        par.part_size = 0;
        strcpy(par.part_name, "");
        par.part_type = '\0';
        par.part_fit = '\0';
        par.part_status = '\0';
        mbr.mbr_partition_1 = mbr.mbr_partition_2 = mbr.mbr_partition_3 = mbr.mbr_partition_4 = par;

        fseek(file, 0, SEEK_SET);
        fwrite(&mbr, sizeof(Mbr), 1, file);

        i = 0;
        while(i != tam){
            fwrite(&buff, 1024, 1, file);
            i++;
        }

        fclose(file);
        cout << "DISK CREATED SUCCESSFULLY"<<endl;
    }else{
        cout << "$Error: The disk already exist"<< endl;
    }
}