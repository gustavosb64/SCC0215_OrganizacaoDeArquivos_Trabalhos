#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./functions.h"

void create_table_cmd(int f_type) {

    char aux_delimiters[1] = " ";

    char *f_input_csv = readline(stdin, aux_delimiters);
    char *f_output_bin = readline(stdin, aux_delimiters);

    write_bin_from_csv(f_input_csv, f_output_bin, f_type);
    binarioNaTela(f_output_bin);

    free(f_input_csv);
    free(f_output_bin);
    return;
}

void select_full_cmd(int f_type) {
    char aux_delimiters[1] = " ";
    char *f_bin = readline(stdin, aux_delimiters);

    if (read_all_reg_from_bin(f_bin, f_type))
        printf("Falha no processamento do arquivo.\n");

    free(f_bin);
}

void select_rrn_cmd() {
    char aux_delimiters[1] = " ";
    char *f_bin = readline(stdin, aux_delimiters);
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
    char aux_delimiters[1] = " ";

    char *f_bin = readline(stdin, aux_delimiters);
    int n;
    scanf("%d\n", &n);
    char** conditions = malloc(n*sizeof(char*));

    aux_delimiters[0] = '\0';

    char* line;
    for (int i=0; i<n; i++) {
        line = readline(stdin, aux_delimiters);
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

    // Lendo os respectivos inputs do stdin
    int operation;
    scanf("%d\n", &operation);
    char aux_delimiters[1] = " ";
    char *f_type_str = readline(stdin, aux_delimiters);
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

    free(f_type_str);
    return 0;
}
