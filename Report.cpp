//
// Created by esdras-toc on 9/08/22.
//
#include "iostream"
#include "Structures.h"
#include <graphviz/gvc.h>
using namespace std;

bool Exist(string path);

string ReportLogicPartitionMbr(Ebr ebr){
    string content = "<table class=\"table table-success table-striped\">\n";
    content += "            <thead>\n";
    content += "                <tr>PARTICION LOGICA</tr>\n";
    content += "            </thead>\n";
    content += "            <tbody>\n";
    content += "                <tr>\n";
    content += "                    <td>part_status</td>\n";
    content += "                    <td>";
    content += ebr.part_status;
    content += "</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_next</td>\n";
    content += "                    <td>";
    content += ebr.part_next;
    content += "</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_fit</td>\n";
    content += "                    <td>";
    content += ebr.part_fit;
    content += "</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_start</td>\n";
    content += "                    <td>"+ to_string(ebr.part_start)+"</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_size</td>\n";
    content += "                    <td>"+ to_string(ebr.part_s)+"</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_name</td>\n";
    content += "                    <td>"+string(ebr.part_name)+"</td>\n";
    content += "                </tr>\n";
    content += "            </tbody>\n";
    content += "        </table>";

    return content;
}

string ReportPartitionMbr(Partition partition, FILE *file){
    if(partition.part_start == 0){
        return "";
    }

    string content = "<table class=\"table\">\n";
    content += "            <thead>\n";
    content += "                <tr>PARTICION</tr>\n";
    content += "            </thead>\n";
    content += "            <tbody>\n";
    content += "                <tr>\n";
    content += "                    <td>part_status</td>\n";
    content += "                    <td>";
    content += partition.part_status;
    content += "</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_type</td>\n";
    content += "                    <td>";
    content += partition.part_type;
    content += "</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_fit</td>\n";
    content += "                    <td>";
    content += partition.part_fit;
    content += "</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_start</td>\n";
    content += "                    <td>"+ to_string(partition.part_start)+"</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_size</td>\n";
    content += "                    <td>"+ to_string(partition.part_size)+"</td>\n";
    content += "                </tr>\n";
    content += "                <tr>\n";
    content += "                    <td>part_name</td>\n";
    content += "                    <td>"+string(partition.part_name)+"</td>\n";
    content += "                </tr>\n";
    content += "            </tbody>\n";
    content += "        </table>";

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

void ReportMbr(string path){
    if(Exist(path)) {
        FILE *file;
        file = fopen(path.c_str(), "rb+");

        fseek(file, 0, SEEK_SET);
        Mbr mbr;
        fread(&mbr, sizeof(Mbr), 1, file);

        string content = "<table class=\"table\">\n";
        content +=          "            <thead>\n";
        content +=          "                <tr>REPORTE DE MBR</tr>\n";
        content +="            </thead>\n";
        content +="            <tbody>\n";
        content +="                <tr>\n";
        content +="                <td>mbr_tamano</td>\n";
        content +="                <td>"+to_string(mbr.mbr_tamano)+"</td>\n";
        content +="                </tr>\n";
        content +="                <tr>\n";
        content +="                <td>mbr_fecha</td>\n";
        content +="                <td>"+string (mbr.mbr_fecha_creacion)+"</td>\n";
        content +="                </tr>\n";
        content +="                <tr>\n";
        content +="                <td>mbr_disk_signature</td>\n";
        content +="                <td>"+ to_string(mbr.mbr_dsk_signature)+"</td>\n";
        content +="                </tr>\n";
        content +="            </tbody>\n";
        content +="        </table>";

        content += ReportPartitionMbr(mbr.mbr_partition_1, file);
        content += ReportPartitionMbr(mbr.mbr_partition_2, file);
        content += ReportPartitionMbr(mbr.mbr_partition_3, file);
        content += ReportPartitionMbr(mbr.mbr_partition_4, file);

        fclose(file);
        string report = "digraph G{\n";
        report += "content[ label = <";
        report += content;
        report += ">]\n}";

        cout << report << endl;

    }else{
        cout << "$Error: the disk doesn't exist"<<endl;
    }
}