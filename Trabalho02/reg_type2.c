#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./records_utils.h"
#include "./index.h"

#define HEADER_SIZE_TYPE2 189
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

int write_reg_in_bin_type2(FILE *file_bin_w, Vehicle *V){

    if ((*V).cidade != NULL){
        (*V).tamCidade = strlen((*V).cidade);
        (*V).codC5 = '0';
    }
    if ((*V).marca != NULL){
        (*V).tamMarca = strlen((*V).marca);
        (*V).codC6 = '1';
    }
    if ((*V).modelo != NULL){
        (*V).tamModelo = strlen((*V).modelo);
        (*V).codC7 = '2';
    }

    fwrite(&(*V).removido, sizeof(char), 1, file_bin_w);

    // Pula campo de tamanhoRegistro e proxOffset para inserir depois
    fseek(file_bin_w, sizeof(int), SEEK_CUR); 

    // Armazena o byte offset onde os dados do registro começam
    long int start_byte = ftell(file_bin_w);

    fwrite(&(*V).prox.offset, sizeof(long int), 1, file_bin_w);

    fwrite(&(*V).id, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).ano, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).qtt, sizeof(int), 1, file_bin_w);

    if ((*V).sigla == NULL){
        (*V).sigla = (char *) calloc (3, sizeof(char));
        strcpy((*V).sigla, "$$");
    }
    fwrite((*V).sigla, sizeof(char), 2, file_bin_w);

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

    // Armazena o último byte escrito e calcula o tamanho do registro
    long int end_byte = ftell(file_bin_w);
    (*V).tamanhoRegistro = end_byte - start_byte;

    // Retorna o ponteiro do arquivo nas posições reservadas ao tamanho e o atualiza 
    fseek(file_bin_w, start_byte - sizeof(int), SEEK_SET);
    fwrite(&(*V).tamanhoRegistro, sizeof(int), 1, file_bin_w);

    // Posiciona o ponteiro do arquivo ao final do registro escrito
    fseek(file_bin_w, 0, SEEK_END);

    return 0;
}

int read_reg_from_bin_type2(FILE *file_bin_r, Vehicle *V, long int *offset){

    // Colocando o ponteiro do arquivo no início do registro a ser buscado
    fseek(file_bin_r, (*offset), SEEK_SET);

    // Contador de bytes utilizado para garantir que não se ultrapasse
    // o limite do registro
    int byte_counter = 0;

    // Usados na leitura para checagem dos dados
    char aux_char;

    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*V).removido, sizeof(char), 1, file_bin_r))
        return 1;

    fread(&(*V).tamanhoRegistro, sizeof(int), 1, file_bin_r);
    fread(&(*V).prox.offset, sizeof(long int), 1, file_bin_r);
    fread(&(*V).id, sizeof(int), 1, file_bin_r);
    fread(&(*V).ano, sizeof(int), 1, file_bin_r);
    fread(&(*V).qtt, sizeof(int), 1, file_bin_r);

    (*V).sigla = (char *) malloc(2 * sizeof(char));
    fread((*V).sigla, sizeof(char), 2, file_bin_r);

    byte_counter += sizeof(int)*4 + sizeof(long int) + sizeof(char)*3;

    for(int i=0; i<3; i++){

        // Caso a quantidade de bytes restantes ultrapasse o tamanhoRegistro,
        // não há espaço para outro campo ter sido armazenado
        if (byte_counter >= (*V).tamanhoRegistro){
            // Retorna o ponteiro do arquivo para o início do registro
            (*offset) = ftell(file_bin_r);

            return 0;
        }

        // Avança o ponteiro o tamanho de um inteiro para ler o caractere de 
        // descrição simplificada do campo
        // Caso fread() retorne 0, o arquivo atingiu o fim
        fseek(file_bin_r, 4, SEEK_CUR);

        if (!fread(&aux_char, sizeof(char), 1, file_bin_r)){
            // Retorna o ponteiro do arquivo para o início do registro
            (*offset) = ftell(file_bin_r);
            return 0;
        }
        fseek(file_bin_r, -5, SEEK_CUR); // retorna o ponteiro para antes do inteiro

        switch(aux_char){

            // Lê a cidade
            case '0':
                fread(&(*V).tamCidade, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC5, sizeof(char), 1, file_bin_r);
                (*V).cidade = (char *) malloc((*V).tamCidade * sizeof(char));

                fread((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_r);

                byte_counter += sizeof(char) + sizeof(int) + (*V).tamCidade;
                break;

            // Lê a marca
            case '1':
                fread(&(*V).tamMarca, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC6, sizeof(char), 1, file_bin_r);
                (*V).marca = (char *) malloc((*V).tamMarca * sizeof(char));

                fread((*V).marca, sizeof(char), (*V).tamMarca, file_bin_r);

                byte_counter += sizeof(char) + sizeof(int) + (*V).tamMarca;
                break;

            // Lê o modelo
            case '2':
                fread(&(*V).tamModelo, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC7, sizeof(char), 1, file_bin_r);
                (*V).modelo = (char *) malloc((*V).tamModelo * sizeof(char));

                fread((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_r);

                byte_counter += sizeof(char) + sizeof(int) + (*V).tamModelo;
                break;

        }

    }

    // Retorna o ponteiro do arquivo para o início do registro
    (*offset) = ftell(file_bin_r);

    return 0;

}
