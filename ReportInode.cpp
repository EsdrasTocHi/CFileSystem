//
// Created by esdras-toc on 2/09/22.
//
#include "Structures.h"
#include "iostream"
using namespace std;
void reportPointerBlock(PointerBlock pb, string link, int pointer, string *nodes, string *edges, int istart, int bstart,
                        FILE *file, int dim, bool isFileBlock);
void reportDirBlock(DirBlock db,string link, int pointer, string *nodes, string *edges, int istart, int bstart,
                    FILE *file);
void reportFileBlock(FileBlock fb, string link, int pointer, string *nodes, string *edges);

void reportInodeTree(Inode inode, string link, int pointer, string *nodes, string *edges, int istart, int bstart,
                     FILE *file){
    *nodes += "i"+ to_string(pointer)+"[label=\"Inode "+ to_string(pointer)
              +"| {i_uid|"+ to_string(inode.i_uid)+"}|{i_gid|"+ to_string(inode.i_gid)+"}|{i_s|"
              + to_string(inode.i_s)+"}|\n";
    *nodes +=  "    {i_atime|"+string(inode.i_atime)+"}|{i_ctime|"+string(inode.i_ctime)
               +"}|{i_mtime|"+string(inode.i_mtime)+"}|\n";
    for(int i = 1; i < 16; i++) {
        *nodes += "    {i_block "+ to_string(i)+"|<i"+ to_string(i)+"> "+ to_string(inode.i_block[i-1])
                  +"}|\n";
    }
    *nodes += "{i_type|";
    *nodes += inode.i_type;
    *nodes += "} | {i_perm|"+ to_string(inode.i_perm)+"}\"];";

    if(link != ""){
        *edges += link+"->i"+ to_string(pointer)+";\n";
    }
    //directorio ->dirblocks o pointerblocks
    if(inode.i_type == '0'){
        for(int i = 0; i < 15; i++){
            if(i < 12){
                if(inode.i_block[i] != -1){
                    DirBlock db;
                    fseek(file, bstart+(64*inode.i_block[i]), SEEK_SET);
                    fread(&db, sizeof(DirBlock), 1, file);
                    string l = "i"+ to_string(pointer)+":i"+ to_string(i+1);
                    reportDirBlock(db, l, inode.i_block[i], nodes, edges, istart, bstart, file);
                }
            }else{
                if(inode.i_block[i] != -1){
                    PointerBlock pb;
                    fseek(file, bstart+(64*inode.i_block[i]), SEEK_SET);
                    fread(&pb, sizeof(PointerBlock), 1, file);
                    string l = "i"+ to_string(pointer)+":i"+ to_string(i+1);
                    reportPointerBlock(pb, l, inode.i_block[i], nodes, edges, istart, bstart, file, i-11, false);
                }
            }
        }
    }else{
        for(int i = 0; i < 15; i++){
            if(i < 12){
                if(inode.i_block[i] != -1){
                    FileBlock db;
                    fseek(file, bstart+(64*inode.i_block[i]), SEEK_SET);
                    fread(&db, sizeof(FileBlock), 1, file);
                    string l = "i"+ to_string(pointer)+":i"+ to_string(i+1);
                    reportFileBlock(db, l, inode.i_block[i], nodes, edges);
                }
            }else{
                if(inode.i_block[i] != -1){
                    PointerBlock pb;
                    fseek(file, bstart+(64*inode.i_block[i]), SEEK_SET);
                    fread(&pb, sizeof(PointerBlock), 1, file);
                    string l = "i"+ to_string(pointer)+":i"+ to_string(i+1);
                    reportPointerBlock(pb, l, inode.i_block[i], nodes, edges, istart, bstart, file, i-11, true);
                }
            }
        }
    }
}