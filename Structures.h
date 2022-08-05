//
// Created by esdras-toc on 2/08/22.
//

#ifndef MIA_PROYECTO1_201807373_STRUCTURES_H
#define MIA_PROYECTO1_201807373_STRUCTURES_H
using namespace std;
#include "string"

typedef struct{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
}Partition;

typedef struct{
    int mbr_tamano;
    char mbr_fecha_creacion[19];
    int mbr_dsk_signature;
    char mbr_dsk_fit;
    Partition mbr_partition_1;
    Partition mbr_partition_2;
    Partition mbr_partition_3;
    Partition mbr_partition_4;
}Mbr;

typedef struct{
    char part_status;
    char part_fit;
    int part_start;
    int part_s;
    int part_next;
    char part_name[16];
}Ebr;

#endif //MIA_PROYECTO1_201807373_STRUCTURES_H
