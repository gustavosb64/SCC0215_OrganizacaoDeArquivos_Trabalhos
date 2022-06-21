#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./reg_type1.h"
#include "./records_utils.h"
#include "./index.h"

#define MAX_RRN 97
#define HEADER_SIZE_TYPE1 182
#define TYPE 1
struct header{
    char status;        // consistência do arquivo
    int tamanhoRegistro;    // tamanho do registro (usado apenas no tipo 2)
    union{
        int rrn;            // RRN do último registro logicamente removido (tipo 1)
        long int offset;    // offset do último registro logicamente removido (tipo 2)
    }topo;           
    char descricao[40]; // descrição dos metadados
    char desC1[22];     // descrição detalhada do campo 1
    char desC2[19];     // descrição detalhada do campo 2
    char desC3[24];     // descrição detalhada do campo 3
    char desC4[8];      // descrição detalhada do campo 4
    char codC5;         // descrição simplificada do campo 5
    char desC5[16];     // descrição detalhada do campo 5
    char codC6;         // descrição simplificada do campo 6
    char desC6[18];     // descrição detalhada do campo 6
    char codC7;         // descrição simplificada do campo 7
    char desC7[19];     // descrição detalhada do campo 7
    union{
        int proxRRN;                // próximo RRN disponível
        long int proxByteOffset;    // próximo offset disponível
    }prox;
    int nroRegRem;      // quantidade de registros logicamente removidos
};

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

int initialize_reg_type1(FILE *file_bin_w){

    // Preenche o espaço do registro com '$'
    char c = '$';
    for(int i=0; i<MAX_RRN; i++)
        fwrite(&c, sizeof(char), 1, file_bin_w);

    // Posiciona o ponteiro do arquivo ao início do registro
    fseek(file_bin_w, -MAX_RRN, SEEK_CUR);
    
    return 0;
}

int read_reg_from_bin_type1(FILE *file_bin_r, Vehicle *V, int rrn){

    // Caso o arquivo de registros não esteja consistente
    if (get_status(file_bin_r) != '1'){
        return 2;
    }

    // Colocando o ponteiro do arquivo no início do registro a ser buscado
    fseek(file_bin_r, MAX_RRN*rrn + HEADER_SIZE_TYPE1, SEEK_SET);

    // Contador de bytes utilizado para garantir que não se ultrapasse
    // o limite do registro
    int byte_counter = 0;

    // Usados na leitura para checagem dos dados
    char aux_char;

    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*V).removido, sizeof(char), 1, file_bin_r))
        return 1;

    fread(&(*V).prox.rrn, sizeof(int), 1, file_bin_r);
    fread(&(*V).id, sizeof(int), 1, file_bin_r);
    fread(&(*V).ano, sizeof(int), 1, file_bin_r);
    fread(&(*V).qtt, sizeof(int), 1, file_bin_r);

    (*V).sigla = (char *) malloc(2 * sizeof(char));
    fread((*V).sigla, sizeof(char), 2, file_bin_r);

    byte_counter += sizeof(int)*4 + sizeof(char)*3;

    for(int i=0; i<3; i++){

        // Caso a quantidade de bytes restantes ultrapasse MAX_RRN-5,
        // não há espaço para outro campo ter sido armazenado
        if (byte_counter > MAX_RRN-5) return 0;

        // Avança o ponteiro o tamanho de um inteiro para ler o caractere de 
        // descrição simplificada do campo
        // Caso fread() retorne 0, o arquivo atingiu o fim
        fseek(file_bin_r, 4, SEEK_CUR);
        if (!fread(&aux_char, sizeof(char), 1, file_bin_r)){
            // Retorna o ponteiro do arquivo para o início do registro
            fseek(file_bin_r, -byte_counter-5, SEEK_CUR);
            return 0;
        }
        fseek(file_bin_r, -5, SEEK_CUR); // retorna o ponteiro para antes do inteiro

        // Caso o caractere lido não esteja entre 0 e 2, trata-se de lixo, 
        // e o registro terminou de ser lido
        if (aux_char < '0' || aux_char > '2'){

            // Retorna o ponteiro do arquivo para o início do registro
            fseek(file_bin_r, -byte_counter, SEEK_CUR);
            return 0;
        }

        switch(aux_char){

            // Lê a cidade
            case '0':
                fread(&(*V).tamCidade, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC5, sizeof(char), 1, file_bin_r);
                (*V).cidade = (char *) malloc((*V).tamCidade * sizeof(char));

                fread((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_r);

                byte_counter += 1+4+(*V).tamCidade;
                break;

            // Lê a marca
            case '1':
                fread(&(*V).tamMarca, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC6, sizeof(char), 1, file_bin_r);
                (*V).marca = (char *) malloc((*V).tamMarca * sizeof(char));

                fread((*V).marca, sizeof(char), (*V).tamMarca, file_bin_r);

                byte_counter += 1+4+(*V).tamMarca;
                break;

            // Lê o modelo
            case '2':
                fread(&(*V).tamModelo, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC7, sizeof(char), 1, file_bin_r);
                (*V).modelo = (char *) malloc((*V).tamModelo * sizeof(char));

                fread((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_r);

                byte_counter += 1+4+(*V).tamModelo;
                break;

        }

    }

    // Retorna o ponteiro do arquivo para o início do registro
    fseek(file_bin_r, -byte_counter, SEEK_CUR);

    return 0;

}

int write_reg_in_bin_type1(FILE *file_bin_w, Vehicle *V){

    // Inicializa o espaço dos próximos MAX_RRN bytes com o caractere '$'
    initialize_reg_type1(file_bin_w);

    // Conta quantos bytes devem ser pulados ao final devido ao padding
    int offset = 0;

    if ((*V).cidade != NULL){
        (*V).tamCidade = strlen((*V).cidade);
        (*V).codC5 = '0';
        offset += (sizeof(int) + sizeof(char) + (*V).tamCidade);
    }
    if ((*V).marca != NULL){
        (*V).tamMarca = strlen((*V).marca);
        (*V).codC6 = '1';
        offset += (sizeof(int) + sizeof(char) + (*V).tamMarca);
    }
    if ((*V).modelo != NULL){
        (*V).tamModelo = strlen((*V).modelo);
        (*V).codC7 = '2';
        offset += (sizeof(int) + sizeof(char) + (*V).tamModelo);
    }

    fwrite(&(*V).removido, sizeof(char), 1, file_bin_w);
    fwrite(&(*V).prox.rrn, sizeof(int), 1, file_bin_w);

    fwrite(&(*V).id, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).ano, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).qtt, sizeof(int), 1, file_bin_w);

    if ((*V).sigla == NULL){
        (*V).sigla = (char *) calloc (3, sizeof(char));
        strcpy((*V).sigla, "$$");
    }
    fwrite((*V).sigla, sizeof(char), 2, file_bin_w);

    offset += (sizeof(char) + 4*sizeof(int) + 2*sizeof(char));

    if ((*V).tamCidade){
        fwrite(&(*V).tamCidade, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC5, sizeof(char), 1, file_bin_w);
        fwrite((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_w);
    }

    if ((*V).tamMarca){
        fwrite(&(*V).tamMarca, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC6, sizeof(char), 1, file_bin_w);
        fwrite((*V).marca, sizeof(char), (*V).tamMarca, file_bin_w);
    }

    if ((*V).tamModelo){
        fwrite(&(*V).tamModelo, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC7, sizeof(char), 1, file_bin_w);
        fwrite((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_w);
    }

    // Posiciona o ponteiro do arquivo ao final do registro com padding
    offset = MAX_RRN - offset;
    fseek(file_bin_w, offset, SEEK_CUR);

    return 0;
}

int search_vehicle_rrn(char *filename_in_bin ,int rrn) {

    // Caso haja falha na leitura do arquivo, retorna 1
    FILE *file_bin_r = fopen(filename_in_bin, "rb");
    if (file_bin_r == NULL){
        return 1;
    }
    Vehicle V = initialize_vehicle(1);

    // Caso haja falha na leitura do registro, retorna 2
    if (read_reg_from_bin_type1(file_bin_r, &V, rrn)){
        fclose(file_bin_r);
        return 2;
    }

    // Imprime os dados do veículo
    print_vehicle(V,1);
    printf("\n");

    // Libera a memória alocada durante a leitura
    free_vehicle(&V);
    fclose(file_bin_r);

    return 0;
}

// header_rrn -> topo da pilha
//  header.topo.rrn
int remove_reg_by_rrn_type1(FILE *file_bin_rw, int rrn, int *err){

    // Caso o arquivo de registros não esteja consistente
    if (get_status(file_bin_rw) != '1'){
        *err = 2;
        return -2;
    }

    fseek(file_bin_rw, MAX_RRN*rrn + HEADER_SIZE_TYPE1, SEEK_SET);

    // Caso registro não seja encontrado, retorna sinal de erro 1
    char aux_char;
    if (!fread(&aux_char, sizeof(char), 1, file_bin_rw)){
        *err = 1;
        return -1;
    }
    fseek(file_bin_rw, -1, SEEK_CUR);

    // Armazena o próximo valor da pilha no registro
    int header_rrn = get_list_top(file_bin_rw, TYPE);
    aux_char = '1'; 
    fwrite(&aux_char, sizeof(char), 1, file_bin_rw);
    fwrite(&header_rrn, sizeof(char), 1, file_bin_rw);

    // Atualiza topo da pilha
    update_list(file_bin_rw, TYPE, rrn);

    // Atualiza nroRegRem
    update_nroRegRem(file_bin_rw, 1, '+');

    return 0;
}

void test_remove_reg_type1(int f_type){

    Vehicle V = initialize_vehicle(f_type);
    FILE *file_bin_rw = fopen("meu_binario5.bin", "rb+");

    int rrn = search_index_from_idx("meu_indice5.bin", 999, f_type);
    printf("rrn: %d\n\n",rrn);

    int err = 0;
    remove_reg_by_rrn_type1(file_bin_rw, rrn, &err);

    printf("\n");
    read_reg_from_bin_type1(file_bin_rw, &V, rrn);
    print_vehicle_full(V, f_type);

    /*
    fclose(file_bin_rw);
    file_bin_rw = fopen("meu_binario5.bin", "rb+");
    */


    int header_rrn; 

    fseek(file_bin_rw, 1, SEEK_SET); 
    fread(&header_rrn, sizeof(int), 1, file_bin_rw);
    printf("\nheader: %d\n",header_rrn);

    return;
}

int print_reg_from_bin_by_rrn(char *filename, int rrn){

    FILE *file_bin_r = fopen(filename, "rb");

    Vehicle V = initialize_vehicle(1);

    if (read_reg_from_bin_type1(file_bin_r, &V, rrn)){
        return 1;
    }

    print_vehicle_full(V, 1);
    printf("\n");

    fclose(file_bin_r);
    free_vehicle(&V);

    return 0;
}

int add_new_reg_type1(FILE *file_bin_rw, Vehicle V){

    int rrn;
    int flag_stack = 0;

    rrn = get_list_top(file_bin_rw, TYPE);

    if (rrn == -1)
        rrn = get_prox(file_bin_rw, TYPE);
    else 
        flag_stack = 1;

    long int offset = rrn*MAX_RRN + HEADER_SIZE_TYPE1;
    fseek(file_bin_rw, offset, SEEK_SET);

    if (flag_stack){
        char is_removed; 
        int new_value; 

        // Caso registro não conste como removido
        fread(&is_removed, sizeof(char), 1, file_bin_rw);
        if (is_removed != '1') 
            return -1;

        fread(&new_value, sizeof(int), 1, file_bin_rw);

        // Atualizando o topo da pilha e o nroRegRem
        update_list(file_bin_rw, TYPE, new_value);
        update_nroRegRem(file_bin_rw, TYPE, '-');

        fseek(file_bin_rw, -(sizeof(char)+sizeof(int)), SEEK_CUR);
    }

    write_reg_in_bin_type1(file_bin_rw, &V);

    if (!flag_stack){
        rrn++;
        update_prox(file_bin_rw, TYPE, rrn);
    }

    return 0;
}

int read_id_from_reg_type1(FILE *file_bin_r, int *id, int rrn){
    
    // Caso o arquivo de registros não esteja consistente
    if (get_status(file_bin_r) != '1'){
        return 1;
    }

    // Colocando o ponteiro do arquivo no ID do registro a ser buscado
    long int id_offset = MAX_RRN * rrn + HEADER_SIZE_TYPE1 + sizeof(char)+sizeof(int);
    fseek(file_bin_r, id_offset, SEEK_SET);

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*id), sizeof(int), 1, file_bin_r)) 
        return 2;

    return 0;
}
