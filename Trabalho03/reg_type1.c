#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./reg_type1.h"
#include "./records_utils.h"
#include "./index.h"
#include "./b_tree.h"

#define MAX_RRN 97
#define HEADER_SIZE_TYPE1 182
#define TYPE 1
struct header{
    char status;        // consistência do arquivo
    union{
        int rrn;            // RRN do último registro logicamente removido (tipo 1)
        long int offset;    // offset do último registro logicamente removido (tipo 2)
    }topo;           
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

/*
 * Inicializa um campo de 97 bytes com valor '$'
*/
int initialize_reg_type1(FILE *file_bin_w){

    // Preenche o espaço do registro com '$'
    char c = '$';
    for(int i=0; i<MAX_RRN; i++)
        fwrite(&c, sizeof(char), 1, file_bin_w);

    // Posiciona o ponteiro do arquivo ao início do registro
    fseek(file_bin_w, -MAX_RRN, SEEK_CUR);
    
    return 0;
}

/*
 * Lê um registro tipo 1 de um arquivo binário de dados pelo rrn
*/
int read_reg_from_bin_type1(FILE *file_bin_r, Vehicle *V, int rrn){

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
    if ((*V).removido == '1')
        return 0;

    fread(&(*V).prox.rrn, sizeof(int), 1, file_bin_r);
    fread(&(*V).id, sizeof(int), 1, file_bin_r);
    fread(&(*V).ano, sizeof(int), 1, file_bin_r);
    fread(&(*V).qtt, sizeof(int), 1, file_bin_r);

    (*V).sigla = (char *) malloc(2 * sizeof(char));
    fread((*V).sigla, sizeof(char), 2, file_bin_r);

    byte_counter += sizeof(int)*4 + sizeof(char)*3;

    /* 
     * Itera 3 vezes, referente aos 3 possíveis campos de 
     * tamanho variável de Vehicle
    */
    for(int i=0; i<3; i++){

        // Caso a quantidade de bytes restantes ultrapasse MAX_RRN-5,
        // não há espaço para outro campo ter sido armazenado, então retorna
        if (byte_counter > MAX_RRN-5) return 0;

        // Avança o ponteiro o tamanho de um inteiro para ler o caractere de 
        // descrição simplificada do campo
        // Caso fread() retorne 0, o arquivo atingiu o fim e retorna
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

        // Compara o aux_char para realizar a leitura correta de cada campo
        // de tamanho variável
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

/*
 * Escreve um registro tipo 1 em um arquivo binário de dados
*/
int write_reg_in_bin_type1(FILE *file_bin_w, Vehicle *V){

    // Inicializa o espaço dos próximos MAX_RRN bytes com o caractere '$'
    initialize_reg_type1(file_bin_w);

    // Conta quantos bytes devem ser pulados ao final devido ao padding
    int offset = 0;

    // Checa se os campos de tamanho variável existem para tratá-los de maneira
    // correta
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

    // Caso sigla esteja vazia, preenche o campo com lixo
    if ((*V).sigla == NULL){
        (*V).sigla = (char *) calloc (3, sizeof(char));
        strcpy((*V).sigla, "$$");
    }
    fwrite((*V).sigla, sizeof(char), 2, file_bin_w);

    offset += (sizeof(char) + 4*sizeof(int) + 2*sizeof(char));

    // Escreve cidade, caso haja
    if ((*V).tamCidade){
        fwrite(&(*V).tamCidade, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC5, sizeof(char), 1, file_bin_w);
        fwrite((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_w);
    }

    // Escreve marca, caso haja
    if ((*V).tamMarca){
        fwrite(&(*V).tamMarca, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC6, sizeof(char), 1, file_bin_w);
        fwrite((*V).marca, sizeof(char), (*V).tamMarca, file_bin_w);
    }

    // Escreve modelo, caso haja
    if ((*V).tamModelo){
        fwrite(&(*V).tamModelo, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC7, sizeof(char), 1, file_bin_w);
        fwrite((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_w);
    }

    /* 
     * Como o registro tem tamanho fixo, ao terminar a escrita, o ponteiro 
     * pode não estar ao final do registro. Utiliza o offset para posicioná-lo 
    */
    offset = MAX_RRN - offset;
    fseek(file_bin_w, offset, SEEK_CUR);

    return 0;
}

/*
 * Busca veículo tipo 1 pelo seu RRN
*/
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

/*
 * Remove um registro tipo 1 do arquivo de dados pelo RRN
*/
int remove_reg_by_rrn(FILE *file_bin_rw, int rrn, Header *header){

    // Posicionando o ponteiro no registro a ser deletado
    long int offset = rrn*MAX_RRN + HEADER_SIZE_TYPE1;
    fseek(file_bin_rw, offset, SEEK_SET);

    // Checa se o arquivo já não está removido
    char is_removed;
    fread(&is_removed, sizeof(char), 1, file_bin_rw);
    if (is_removed == '1') 
        return -1;
    fseek(file_bin_rw, -sizeof(char), SEEK_CUR);

    // Marca registro como removido
    is_removed = '1';
    fwrite(&is_removed, sizeof(char), 1, file_bin_rw);
    
    // Atualiza pilha
    int stack_top = header->topo.rrn;
    fwrite(&stack_top, sizeof(int), 1, file_bin_rw);

    // Atualiza header
    header->topo.rrn = rrn;
    header->nroRegRem += 1;
    
    return 0;
}

/*
 * Imprime um registro tipo 1 o lendo do arquivo de dados pelo RRN
*/
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

/*
 * Adiciona um novo registro tipo 1 ao arquivo de dados
*/
int add_new_reg_type1(FILE *file_bin_rw, Vehicle V, int *rrn, Header *header){

    int flag_stack = 0; // flag para indicar se houve reaproveitamento de espaço

    // Checa se a pilha de registros removidos não está vazia
    if (header->topo.rrn == -1)
        // Caso esteja vazia, rrn recebe o próximo RRN disponível
        (*rrn) = header->prox.proxRRN;
    else{ 
        // Caso não esteja vazia, rrn recebe valor da pilha e flag_stack é marcada 
        (*rrn) = header->topo.rrn;
        flag_stack = 1;
    }

    // Posiciona o cursor onde será inserido o novo registro
    long int offset = (*rrn)*MAX_RRN + HEADER_SIZE_TYPE1;
    fseek(file_bin_rw, offset, SEEK_SET);


    // Caso haja espaço a ser reaproveitado
    if (flag_stack){
        char is_removed; 
        int new_stack_top; 

        // Caso registro não conste como removido, retorna
        fread(&is_removed, sizeof(char), 1, file_bin_rw);
        if (is_removed != '1'){
            return -1;
        }

        // Atualiza o topo da pilha e o nroRegRem
        fread(&new_stack_top, sizeof(int), 1, file_bin_rw);
        header->topo.rrn = new_stack_top;
        header->nroRegRem = header->nroRegRem - 1;

        /* 
         * Retorna o ponteiro ao início do registro
         *  Este retorno é necessário para se reutilizar a função de
         *  escrita de registro tipo 1 implementada anteriormente
        */
        fseek(file_bin_rw, -(sizeof(char)+sizeof(int)), SEEK_CUR);
    }

    /* 
     * Escreve novo registro
     *  Para favorecer a modularização do programa, optou-se por reutilizar
     *  a função write_reg_in_bin_type1
    */
    write_reg_in_bin_type1(file_bin_rw, &V);

    // Caso não tenha tido reaproveitamento de espaço, proxRRN é atualizado
    if (!flag_stack){
        int new_proxRRN = (*rrn)+1;
        header->prox.proxRRN = new_proxRRN;
    }

    return 0;
}

/*
 * Lê um id de um registro tipo 1 dado seu RRN
*/
int read_id_from_reg_type1(FILE *file_bin_r, int *id, int rrn, Header *header){
    
    // Colocando o ponteiro do arquivo no ID do registro a ser buscado
    long int id_offset = MAX_RRN * rrn + HEADER_SIZE_TYPE1;
    fseek(file_bin_r, id_offset, SEEK_SET);

    // Caso não haja mais registros a serem lidos, retorna sinal de erro 3
    char is_removed;
    if (!fread(&is_removed, sizeof(char), 1, file_bin_r)){
        return 2;
    }

    // Caso o registro esteja removido, retorna 
    if (is_removed == '1'){
        (*id) = -1;
        return 0;
    }

    // Posicionando cursor no ID
    fseek(file_bin_r, sizeof(int), SEEK_CUR);

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 3
    if (!fread(&(*id), sizeof(int), 1, file_bin_r)) 
        return 3;

    return 0;
}

/*
 * Atualiza campo de registro tipo 1 identificado por seu rrn
 * com dados presentes em Vehicle V
*/
int update_reg_type1(FILE *file_bin_rw, Vehicle V, int rrn){

    // Posicionando o ponteiro no registro a ser deletado
    long int offset = rrn*MAX_RRN + HEADER_SIZE_TYPE1;
    fseek(file_bin_rw, offset, SEEK_SET);

    // Checa se registro não está removido
    char is_removed;
    fread(&is_removed, sizeof(char), 1, file_bin_rw);
    if(is_removed == '1')
        return -1;
    fseek(file_bin_rw, -1, SEEK_CUR);

    write_reg_in_bin_type1(file_bin_rw, &V);
    
    return 0;
}
