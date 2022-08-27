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