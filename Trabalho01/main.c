#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./functions.h"

void create_table_cmd(int f_type) {
    char *f_input_csv = readfield(stdin);
    char *f_output_bin = readfield(stdin);

    write_bin_from_csv(f_input_csv, f_output_bin, f_type);
    binarioNaTela(f_output_bin);

    free(f_input_csv);
    free(f_output_bin);
    return;
}

void select_full_cmd(int f_type) {
    char *f_bin = readfield(stdin);

    read_all_reg_from_bin(f_bin, f_type);

    free(f_bin);
}

void select_rrn_cmd() {
    char *f_bin = readfield(stdin);
    int rrn;
    scanf("%d", &rrn);
    
    switch(search_vehicle_rrn(f_bin, rrn)){
        case 1:
            printf("Falha no processamento do arquivo.\n");
            break;

        case 2:
            printf("Registro inexistente.\n");
            break;

        default:
            break;
    }

    free(f_bin);
}

void select_where_cmd(int f_type) {
    char *f_bin = readfield(stdin);
    int n;
    scanf("%d\n", &n);
    char** conditions = malloc(n*sizeof(char*));

    char* line;
    for (int i=0; i<n; i++) {
        line = readline(stdin);
        conditions[i] = line;
    }

    read_condition_reg_from_bin(f_bin, f_type, conditions, n);

    for (int i=0; i<n; i++) {
        free(conditions[i]);
    }
    free(conditions);
    free(f_bin);
}

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
    // Lendo os respectivos inputs do stdin
    int operation;
    scanf("%d\n", &operation);
    char *f_type_str = readfield(stdin);
    int f_type = f_type_str[strlen(f_type_str)-1] - '0';


    switch(operation) {
        case 1: {
            create_table_cmd(f_type);
            break;
        }

        case 2: {
            select_full_cmd(f_type);
            break;
        }

        case 3: {
            select_where_cmd(f_type);
            break;
        }

        case 4: {
            select_rrn_cmd();
            break;
        }

        default: {
            break;
        }
    }


    /*
    write_bin_from_csv_type1("frota.csv", "frota_1.bin");
    read_bin_all_reg_type1("frota_1.bin");
    */

    /*
    write_bin_from_csv_type2("frota.csv", "frota_2.bin");
    read_bin_all_reg_type2("frota_2.bin");
    */

    //-write_bin_from_csv("frota.csv", "frota_1.bin", 1);
//    read_bin_all_reg_type2("frota_2.bin");
    //-read_all_reg_from_bin("frota_1.bin",1);

//    fclose(File);

    free(f_type_str);
    return 0;
}
