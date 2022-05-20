#include <stdio.h>
#include <stdlib.h>
#include "./functions.h"

int main(int argc, char *argv[]){

    // Lendo os respectivos inputs do argv[]
    /*
    int operation = atoi(argv[1]);
    int f_type = atoi(argv[2]);
    char *f_input_csv = argv[3];
    char *f_output_bin = argv[4];

    char *filename = readline(stdin);
    write_header_type1(filename);

    read_header_type1(filename);
    */

    /*
    write_bin_from_csv_type1("frota.csv", "frota_1.bin");
    read_bin_all_reg_type1("frota_1.bin");
    */

    /*
    write_bin_from_csv_type2("frota.csv", "frota_2.bin");
    read_bin_all_reg_type2("frota_2.bin");
    */

    write_bin_from_csv("frota.csv", "frota_1.bin", 1);
//    read_bin_all_reg_type2("frota_2.bin");
    read_all_reg_from_bin("frota_1.bin",1);

//    fclose(File);
    return 0;

}
