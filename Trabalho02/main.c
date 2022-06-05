#include <stdio.h>
#include <stdlib.h>
#include "./records.h"
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
    write_idx_file_from_bin("binario3.bin", "indices3.bin", f_type);
    printf("###########\n");

    read_all_indices_from_idx("indices3.bin", f_type);

    /*
    printf("###########\n");

    FILE *file_bin_r = fopen("binario1.bin", "rb");
    Vehicle V = initialize_vehicle(f_type);

    read_all_reg_from_bin("binario1.bin", f_type);
    read_reg_from_bin_type1(file_bin_r, &V, 999);

    print_vehicle_full(V, f_type);
    */
    return 0;
}
