//
// Created by esdras-toc on 9/08/22.
//
#include "iostream"
#include "Structures.h"
#include <graphviz/gvc.h>
using namespace std;

bool Exist(string path);

string ReportLogicPartitionMbr(Partition extendedPar){
    return "";
}

string ReportPartitionMbr(Partition par){
return "";
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

        if(mbr.mbr_partition_1.part_start != 0){
            content += ReportPartitionMbr(mbr.mbr_partition_1);
        }
        if(mbr.mbr_partition_2.part_start != 0){
            content += ReportPartitionMbr(mbr.mbr_partition_2);
        }
        if(mbr.mbr_partition_3.part_start != 0){
            content += ReportPartitionMbr(mbr.mbr_partition_3);
        }
        if(mbr.mbr_partition_4.part_start != 0){
            content += ReportPartitionMbr(mbr.mbr_partition_4);
        }

        string report = "digraph G{\n";
        report += "content[ label = <";
        report += content;
        report += ">]\n}";

        cout << report << endl;

    }else{
        cout << "$Error: the disk doesn't exist"<<endl;
    }
}