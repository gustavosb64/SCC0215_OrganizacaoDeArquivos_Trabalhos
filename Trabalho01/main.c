#include <stdio.h>
#include <stdlib.h>
#include "./functions.h"

int main(int argc, char *argv[]){

    /*
    char *filename = readline(stdin);
    write_header_type1(filename);

    read_header_type1(filename);
    */

//    write_bin_from_csv_type1("frota.csv");

//    read_bin_all_reg_type1("frota_1.bin");

    write_bin_from_csv_type2("frota.csv");

    read_bin_all_reg_type2("frota_2.bin");
//    fclose(File);
    return 0;

}
