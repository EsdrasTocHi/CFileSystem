//
// Created by esdras-toc on 9/08/22.
//
#include "iostream"
#include "Structures.h"
#include "graphviz/gvc.h"
#include "stdlib.h"
using namespace std;

bool Exist(string path);

string getExtension(string path){
    string res = "";
    for(int i = path.length()-1; i >= 0; i--){
        if(path[i] == '.'){
            break;
        }
        res = path[i]+res;
    }

    return res;
}

string removeExtension(string path){
    string res = "";
    bool write = false;
    for(int i = path.length()-1; i >= 0; i--){
        if(write){
            res = path[i]+res;
            continue;
        }

        if(path[i] == '.'){
            write = true;
        }
    }

    return res;
}
bool saveImageGV(string file_path, string content){
    string ext = getExtension(file_path);
    FILE *dot;
    dot = fopen("temp.dot", "w");
    fwrite(content.c_str(), content.length(), 1, dot);
    fclose(dot);
    system(("dot -T"+ getExtension(file_path)+" temp.dot -o "+ file_path).c_str());
}

void reportInodeTree(Inode inode, string link, int pointer, string *nodes, string *edges, int istart, int bstart,
                     FILE *file);
string ReportLogicPartitionMbr(Ebr partition){
    string content = "<tr>\n";
    content += "            <td>Particion logica</td>\n";
    content +=       "        </tr>\n";
    content +=       "        <tr>\n";
    content +=       "            <td>\n";
    content +=       "                <TABLE>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_status</TD>\n";
    content +=       "                        <td>";
    content += partition.part_status;
    content +=       "</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_next</TD>\n";
    content +=       "                        <td>";
    content +=       to_string(partition.part_next);
    content +=       "</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_fit</TD>\n";
    content +=       "                        <td>";
    content +=       partition.part_fit;
    content +=       "</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_start</TD>\n";
    content +=       "                        <td>"+to_string(partition.part_start)+"</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_size</TD>\n";
    content +=       "                        <td>"+to_string(partition.part_s)+"</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_name</TD>\n";
    content +=       "                        <td>"+string(partition.part_name)+"</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                </TABLE>\n";
    content +=       "            </td>\n";
    content +=       "        </tr>";

    return content;
}

string ReportPartitionMbr(Partition partition, FILE *file){
    if(partition.part_start == 0){
        return "";
    }

    string content = "<tr>\n";
    content += "            <td>Particion</td>\n";
    content +=       "        </tr>\n";
    content +=       "        <tr>\n";
    content +=       "            <td>\n";
    content +=       "                <TABLE>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_status</TD>\n";
    content +=       "                        <td>";
    content += partition.part_status;
    content +=       "</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_type</TD>\n";
    content +=       "                        <td>";
    content +=       partition.part_type;
    content +=       "</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_fit</TD>\n";
    content +=       "                        <td>";
    content +=       partition.part_fit;
    content +=       "</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_start</TD>\n";
    content +=       "                        <td>"+to_string(partition.part_start)+"</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_size</TD>\n";
    content +=       "                        <td>"+to_string(partition.part_size)+"</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                    <TR>\n";
    content +=       "                        <TD>part_name</TD>\n";
    content +=       "                        <td>"+string(partition.part_name)+"</td>\n";
    content +=       "                    </TR>\n";
    content +=       "                </TABLE>\n";
    content +=       "            </td>\n";
    content +=       "        </tr>";

    if(partition.part_type == 'e'){
        Ebr ebr;
        int pointer = partition.part_start;

        while(true){
            fseek(file, pointer, SEEK_SET);
            fread(&ebr, sizeof(Ebr), 1, file);
            if(ebr.part_start == 0){
                break;
            }

            content += ReportLogicPartitionMbr(ebr);

            if(ebr.part_next == -1){
                break;
            }

            pointer = ebr.part_next;
        }
    }

    return content;
}

void ReportMbr(string path, string imagePath){
    if(Exist(path)) {
        FILE *file;
        file = fopen(path.c_str(), "rb+");

        fseek(file, 0, SEEK_SET);
        Mbr mbr;
        fread(&mbr, sizeof(Mbr), 1, file);

        string content = "<TR>\n";
        content += "            <TD>Reporte de MBR</TD>\n";
        content +=       "        </TR>\n";
        content +=       "        <tr>\n";
        content +=       "            <td>\n";
        content +=       "                <TABLE>\n";
        content +=       "                    <TR>\n";
        content +=       "                        <TD>mbr_tamano</TD>\n";
        content +=       "                        <td>"+ to_string(mbr.mbr_tamano)+"</td>\n";
        content +=       "                    </TR>\n";
        content +=       "                    <TR>\n";
        content +=       "                        <TD>fecha_creacion</TD>\n";
        content +=       "                        <td>"+ string(mbr.mbr_fecha_creacion)+"</td>\n";
        content +=       "                    </TR>\n";
        content +=       "                    <TR>\n";
        content +=       "                        <TD>dsk_signature</TD>\n";
        content +=       "                        <td>"+ to_string(mbr.mbr_dsk_signature)+"</td>\n";
        content +=       "                    </TR>\n";
        content +=       "                </TABLE>\n";
        content +=       "            </td>\n";
        content +=       "        </tr>";

        content += ReportPartitionMbr(mbr.mbr_partition_1, file);
        content += ReportPartitionMbr(mbr.mbr_partition_2, file);
        content += ReportPartitionMbr(mbr.mbr_partition_3, file);
        content += ReportPartitionMbr(mbr.mbr_partition_4, file);

        content = "<table>\n"+content+"</table>";
        fclose(file);
        string report = "digraph G{\nnode [shape=plaintext]\n";
        report += "content[ label = <";
        report += content;
        report += ">];\n}";

        saveImageGV(imagePath, report);
    }else{
        cout << "$Error: the disk doesn't exist"<<endl;
    }
}

void ReportBMInode(MountedPartition *mountedPartition, string path){
    int start, inodes;
    SuperBlock sp;
    FILE *file = fopen(mountedPartition->path.c_str(), "rb+");
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
        fseek(file, start, SEEK_SET);
        fread(&sp, sizeof(SuperBlock), 1, file);
    }else{
        start = mountedPartition->par.part_start;
        fseek(file, start, SEEK_SET);
        fread(&sp, sizeof(SuperBlock), 1, file);
    }

    if(sp.s_filesystem_type == 0){
        fclose(file);
        cout << "$Error: bitmap does not exists" << endl;
        return;
    }

    string content = "";
    char bit;
    fseek(file, sp.s_bm_inode_start, SEEK_SET);
    int j = 1;
    for(int i = sp.s_bm_inode_start; i < sp.s_bm_inode_start+sp.s_inodes_count; i++){
        fread(&bit, 1, 1, file);
        content += bit;
        content += " ";

        if(j%20 == 0){
            content += "\n";
        }
        j++;
    }

    fclose(file);

    file = fopen(path.c_str(), "w");
    fwrite(content.c_str(), content.length(), 1, file);
    fclose(file);

    cout << "REPORT COMPLETED" << endl;
}

void ReportBMBlocks(MountedPartition *mountedPartition, string path){
    int start;
    SuperBlock sp;
    FILE *file = fopen(mountedPartition->path.c_str(), "rb+");
    if(mountedPartition->isLogic){
        start = mountedPartition->logicPar.part_start;
        fseek(file, start, SEEK_SET);
        fread(&sp, sizeof(SuperBlock), 1, file);
    }else{
        start = mountedPartition->par.part_start;
        fseek(file, start, SEEK_SET);
        fread(&sp, sizeof(SuperBlock), 1, file);
    }

    if(sp.s_filesystem_type == 0){
        fclose(file);
        cout << "$Error: bitmap does not exists" << endl;
        return;
    }

    string content = "";
    char bit;
    fseek(file, sp.s_bm_block_start, SEEK_SET);
    int j = 1;
    for(int i = sp.s_bm_block_start; i < sp.s_bm_block_start+sp.s_blocks_count; i++){
        fread(&bit, 1, 1, file);
        content += bit;
        content += " ";

        if(j%20 == 0){
            content += "\n";
        }
        j++;
    }

    fclose(file);

    file = fopen(path.c_str(), "w");
    fwrite(content.c_str(), content.length(), 1, file);
    fclose(file);

    cout << "REPORT COMPLETED" << endl;
}

void reportInode(Inode inode, string *nodes, string *edges, int lastInode, int actualInode){
    *nodes += "<tr>\n";
    *nodes +="                            <td>i_uid</td>\n";
    *nodes += "                            <td>"+ to_string(inode.i_uid)+"</td>\n";
    *nodes += "                        </tr>\n";
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>i_gid</td>\n";
    *nodes += "                            <td>"+ to_string(inode.i_gid)+"</td>\n";
    *nodes += "                        </tr>\n";
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>i_s</td>\n";
    *nodes += "                            <td>"+to_string(inode.i_s)+"</td>\n";
    *nodes += "                        </tr>\n";
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>i_atime</td>\n";
    *nodes += "                            <td>"+string(inode.i_atime)+"</td>\n";
    *nodes += "                        </tr>\n";
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>i_ctime</td>\n";
    *nodes += "                            <td>"+string(inode.i_ctime)+"</td>\n";
    *nodes += "                        </tr>\n";
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>i_mtime</td>\n";
    *nodes += "                            <td>"+string(inode.i_mtime)+"</td>\n";
    *nodes += "                        </tr>\n";
    for(int i = 0; i < 15; i++) {
        *nodes += "                        <tr>\n";
        *nodes += "                            <td>i_block"+ to_string(i+1)+"</td>\n";
        *nodes += "                            <td>"+ to_string(inode.i_block[i])+"</td>\n";
        *nodes += "                        </tr>\n";
    }
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>i_type</td>\n";
    *nodes += "                            <td>";
    *nodes += inode.i_type;
    *nodes += "</td>\n";
    *nodes += "                        </tr>\n";
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>i_perm</td>\n";
    *nodes += "                            <td>"+ to_string(inode.i_perm)+"</td>\n";
    *nodes += "                        </tr>\n";

    if(lastInode != -1) {
        *edges += "i" + to_string(lastInode) + "->i" + to_string(actualInode)+";\n";
    }
}

void reportFileBlock(FileBlock fb, string link, int pointer, string *nodes, string *edges){
    string c = "";
    if(string(fb.b_content).length() > 64) {
        for (int i = 0; i < 64; i++) {
            c += fb.b_content[i];
        }
    }else{
        c = string(fb.b_content);
    }
    *nodes += "b"+ to_string(pointer)+"[label=<\n";
    *nodes += "        <table>\n";
    *nodes += "            <tr>\n";
    *nodes += "                <td>\n";
    *nodes += "                    <table>\n";
    *nodes += "                        <tr>\n";
    *nodes += "                            <td>FileBlock</td>\n";
    *nodes += "                            <td>"+ to_string(pointer)+"</td>\n";
    *nodes += "                        </tr>\n";
    *nodes += "                    </table>\n";
    *nodes += "                </td>\n";
    *nodes += "            </tr>\n";
    *nodes += "            <tr>\n";
    *nodes += "                <td>"+c+"</td>\n";
    *nodes += "            </tr>\n";
    *nodes += "        </table>\n";
    *nodes += "    >];\n";

    *edges += link + "->" + "b"+ to_string(pointer)+";\n";
}

void reportDirBlock(DirBlock db,string link, int pointer, string *nodes, string *edges, int istart, int bstart,
                    FILE *file){
    *nodes+="b"+ to_string(pointer)+"[label=\"DirBlock "+ to_string(pointer);
    for(int i = 1; i < 5; i++){
        *nodes += "|{"+string(db.b_content[i-1].b_name)+"|<b"+ to_string(i)+">"+ to_string(db.b_content[i-1].b_inodo)+"}";
    }
    *nodes += "\"];\n";

    *edges += link+"->b"+ to_string(pointer)+";\n";
    for(int i = 0; i < 4; i++){
        if(string(db.b_content[i].b_name) != "." && string(db.b_content[i].b_name) != "..") {
            if(db.b_content[i].b_inodo == -2){
                continue;
            }
            if (db.b_content[i].b_inodo != -1) {
                Inode aux;
                fseek(file, istart + (sizeof(Inode) * db.b_content[i].b_inodo), SEEK_SET);
                fread(&aux, sizeof(Inode), 1, file);

                string link = "b" + to_string(pointer) + ":<b" + to_string(i + 1) + ">";
                reportInodeTree(aux, link, db.b_content[i].b_inodo, nodes, edges, istart, bstart, file);
            }
        }
    }
}

void reportPointerBlock(PointerBlock pb, string link, int pointer, string *nodes, string *edges, int istart, int bstart,
                        FILE *file, int dim, bool isFileBlock){
    *nodes += "b"+ to_string(pointer)+"[label=\"PointerBlock "+ to_string(pointer);
    for(int i = 0; i < 16; i++){
        *nodes += "|{pointer "+ to_string(i+1)+"|<b"+ to_string(i+1)+">"+ to_string(pb.b_pointers[i])+"}";
    }
    *nodes += "\"];\n";

    *edges += link + "->b"+ to_string(pointer)+";\n";

    if(dim == 1) {
        if (isFileBlock) {
            for (int i = 0; i < 16; i++) {
                if (pb.b_pointers[i] != -1) {
                    FileBlock fileBlock;
                    fseek(file, bstart + (64 * pb.b_pointers[i]), SEEK_SET);
                    fread(&fileBlock, sizeof(FileBlock), 1, file);
                    string link = "b" + to_string(pointer) + ":b" + to_string(i + 1);
                    reportFileBlock(fileBlock, link, pb.b_pointers[i], nodes, edges);
                }
            }
        } else {
            DirBlock fileBlock;
            for (int i = 0; i < 16; i++) {
                if (pb.b_pointers[i] != -1) {
                    fseek(file, bstart + (64 * pb.b_pointers[i]), SEEK_SET);
                    fread(&fileBlock, sizeof(DirBlock), 1, file);
                    string link = "b" + to_string(pointer) + ":b" + to_string(i + 1);
                    reportDirBlock(fileBlock, link, pb.b_pointers[i], nodes, edges, istart, bstart, file);
                }
            }
        }
    }else{
        PointerBlock fileBlock;
        for (int i = 0; i < 16; i++) {
            if (pb.b_pointers[i] != -1) {
                fseek(file, bstart + (64 * pb.b_pointers[i]), SEEK_SET);
                fread(&fileBlock, sizeof(PointerBlock), 1, file);
                string link = "b" + to_string(pointer) + ":b" + to_string(i + 1);
                reportPointerBlock(fileBlock, link, pb.b_pointers[i], nodes, edges, istart, bstart, file, dim-1, isFileBlock);
            }
        }
    }
}

void ReportInodes(MountedPartition partition, string path){
    Inode aux;
    int lastInode = -1;
    string nodes, edges;

    int start;
    if(partition.isLogic){
        start = partition.logicPar.part_start;
    }else{
        start = partition.par.part_start;
    }

    FILE *file = fopen(partition.path.c_str(), "rb+");
    SuperBlock sb;

    fseek(file, start, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, file);

    if(sb.s_filesystem_type == 0){
        cout << "$Error: the partition is not formatted" << endl;
        return;
    }
    fseek(file, sb.s_inode_start, SEEK_SET);
    for(int i = 0; i < sb.s_inodes_count; i++){
        fread(&aux, sizeof(Inode), 1, file);
        if(aux.i_perm == 0){
            continue;
        }
        nodes += "i"+ to_string(i)+"[label = <\n";
        nodes += "        <table>\n";
        nodes += "            <tr>\n";
        nodes += "                <td>Inode "+ to_string(i)+"</td>\n";
        nodes += "            </tr>\n";
        nodes += "            <tr>\n";
        nodes += "                <td>";
        nodes += "                   <table>";
        reportInode(aux, &nodes, &edges, lastInode, i);
        nodes += "</table>\n";
        nodes += "                </td>\n";
        nodes += "            </tr>\n";
        nodes += "        </table>\n";
        nodes += "    >];\n\n";
        lastInode = i;
    }

    fclose(file);

    string graphContent = "digraph G {\n";
    graphContent += "    node [shape=plaintext]\n";
    graphContent +=        "    rankdir=LR;\n"+nodes+edges+"}";

    saveImageGV(path, graphContent);
}

void ReportTree(MountedPartition partition, string path){
    FILE *file = fopen(partition.path.c_str(), "rb+");
    int start;
    if(partition.isLogic){
        start = partition.logicPar.part_start;
    }else{
        start = partition.par.part_start;
    }

    fseek(file, start, SEEK_SET);
    SuperBlock sb;
    fread(&sb, sizeof(SuperBlock), 1, file);
    if(sb.s_filesystem_type == 0){
        fclose(file);
        cout << "$Error: the partition is not formatted"<<endl;
        return;
    }
    Inode root;
    fseek(file, sb.s_inode_start, SEEK_SET);
    fread(&root, sizeof(Inode), 1, file);
    string nodes, edges;

    reportInodeTree(root, "", 0, &nodes, &edges, sb.s_inode_start, sb.s_block_start, file);

    string content = "digraph G {node[shape = record];rankdir = LR;\n"+nodes+edges+"}";

    fclose(file);
    saveImageGV(path, content);
}

void ReportJournaling(MountedPartition partition, string path){
    FILE *file = fopen(partition.path.c_str(), "rb+");
    int start;
    if(partition.isLogic){
        start = partition.logicPar.part_start;
    }else{
        start = partition.par.part_start;
    }

    fseek(file, start, SEEK_SET);
    SuperBlock sb;
    fread(&sb, sizeof(SuperBlock), 1, file);
    if(sb.s_filesystem_type == 0){
        fclose(file);
        cout << "$Error: the partition is not formatted"<<endl;
        return;
    }else if(sb.s_filesystem_type != 3){
        fclose(file);
        cout << "$Error: the partition do not have journaling"<<endl;
        return;
    }

    string content;
    Journal j;
    int counter = 1;
    for(int i = start + sizeof(SuperBlock); i < sb.s_bm_inode_start; i += sizeof(Journal)){
        fseek(file, i, SEEK_SET);
        fread(&j, sizeof(Journal), 1, file);
        if(j.j_type == -1){
            break;
        }
        content = "j"+ to_string(counter)+"[label=\"log "+ to_string(counter)+"|{{operation|"+string(j.j_operation)+
                "}|{type|"+to_string(j.j_type)+"}|{path|"+string(j.j_name)+"}|{content|"+string(j.j_content)
                +"}|{date|"+string(j.j_date)+"}|\n"+"    {userId|"+ to_string(j.j_owner)+"}}\"];\n"+content;
        counter++;
    }

    content = "digraph G {node[shape = record];rankdir = LR;\n"+content+"}";

    fclose(file);
    saveImageGV(path, content);
}