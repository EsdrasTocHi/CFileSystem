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

typedef struct{
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_counts;
    int s_free_inodes_count;
    char s_mtime[19];
    char s_umtime[19];
    int s_mnt_count;
    int s_magic;
    int s_inode_s;
    int s_block_s;
    int s_first_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
}SuperBlock;

typedef struct {
    int i_uid;
    int i_gid;
    int i_s;
    char i_atime[19];
    char i_ctime[19];
    char i_mtime[19];
    int i_block[15];
    //1 = archivo, 0 = carpeta
    char i_type;
    int i_perm;
}Inode;

typedef struct{
    char b_name[12];
    int b_inodo;
}Content;

typedef struct{
    Content b_content[4];
}DirBlock;

typedef struct{
    char b_content[64];
}FileBlock;

typedef struct{
    int b_pointers[16];
}PointerBlock;


/*
 *
 * UTILIDADES
 *
 */
typedef struct{
    Partition par;
    Ebr logicPar;
    bool isLogic;
    string path;
    string id;
}MountedPartition;

typedef struct{
    int id;
    char group[10];
    char name[10];
    char password[10];
}User;

typedef struct{
    User user;
    MountedPartition mountedPartition;
}Sesion;
#endif //MIA_PROYECTO1_201807373_STRUCTURES_H
