#include <stdio.h>
#include <stdlib.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./index.h"
#include "./lista_encadeada.h"

struct vehicle{
    char removido;      // indica se o registro está logicamente removido
    int tamanhoRegistro;    // utilizado apenas por registros tipo 2
    union{
        int rrn;           // armazena o RRN do próximo registro (tipo 1)
        long int offset;   // armazena o offset do próximo registro (tipo 2)
    }prox;
    int id;             // código identificador
    int ano;            // ano de fabricação
    int tamCidade;      // tamanho do campo cidade
    char codC5;         // descrição simplificada do campo 5
    char *cidade;       // nome da cidade
    int qtt;            // quantidade de veículos
    char *sigla;        // sigla do estado no qual o veículo está cadastrado
    int tamMarca;       // tamanho do campo marca
    char codC6;         // descrição simplificada do campo 5
    char *marca;        // nome da marca
    int tamModelo;      // tamanho do campo modelo
    char codC7;         // descrição simplificada do campo 5
    char *modelo;       // nome do modelo
};

int main(int argc, char *argv[]){

    int f_type = 1;
    //test_remove_reg_type1(f_type);

    Vehicle V = initialize_vehicle(f_type);
    write_idx_file_from_bin("./antes/binario1.bin", "indices1.bin", f_type);
    //read_all_indices_from_idx("indices1.bin", f_type);

    //int rrn = search_index_from_idx("./meu_indice5.bin", 555, f_type);
    //printf("rrn: %d\n\n",rrn);

    /*
    FILE *file_bin_r = fopen("./meu_binario5.bin", "rb");
    read_all_reg_from_bin("./depois/binario5.bin", f_type);
    read_reg_from_bin_type1(file_bin_r, &V, rrn);
    print_vehicle_full(V, f_type);


    printf("###########\n");

    read_all_indices_from_idx("indices1.bin", f_type);

    printf("%d\n",search_index_from_idx("indices1.bin", 313, 1));
    printf("###########\n");

    FILE *file_bin_r = fopen("binario1.bin", "rb");

    read_all_reg_from_bin("binario1.bin", f_type);
    read_reg_from_bin_type1(file_bin_r, &V, 999);

    print_vehicle_full(V, f_type);
    */

//    add_new_reg(f_type, "binario1_teste.bin", "indices1.bin", 1001, 2001, 123, "RS", "Porto Alegre", "Chevrolet", "Agile");

//    read_all_reg_from_bin("binario1_teste.bin", f_type);
    return 0;
}
