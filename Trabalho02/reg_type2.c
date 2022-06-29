#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./records_utils.h"
#include "./index.h"

#define HEADER_SIZE_TYPE2 190
#define TYPE 2
struct header{
    char status;        // consistência do arquivo
    int tamanhoRegistro;    // tamanho do registro (usado apenas no tipo 2)
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
 * Escreve um registro tipo 2 em um arquivo binário de dados
*/
int write_reg_in_bin_type2(FILE *file_bin_w, Vehicle *V){

    // Checa se os campos de tamanho variável existem para tratá-los de maneira
    // correta
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

    /* 
     * Caso V.tamanhoRegistro seja maior que 0, indica que a escrita 
     * se trata de uma alteração de um campo. Nestas condições, o tamanho 
     * do campo não pode ser alterado
    */
    if ((*V).tamanhoRegistro > 0){
        fwrite(&(*V).tamanhoRegistro, sizeof(int), 1, file_bin_w);
    }
    else{
        // Pula campo de tamanhoRegistro e proxOffset para inserir depois
        fseek(file_bin_w, sizeof(int), SEEK_CUR); 
    }

    // Armazena o byte offset onde os dados do registro começam
    long int start_byte = ftell(file_bin_w);

    fwrite(&(*V).prox.offset, sizeof(long int), 1, file_bin_w);

    fwrite(&(*V).id, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).ano, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).qtt, sizeof(int), 1, file_bin_w);

    // Caso sigla esteja vazia, preenche o campo com lixo
    if ((*V).sigla == NULL){
        (*V).sigla = (char *) calloc (3, sizeof(char));
        strcpy((*V).sigla, "$$");
    }
    fwrite((*V).sigla, sizeof(char), 2, file_bin_w);

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
     * Caso o tamanho do registro esteja marcado como 0, reliza a 
     * contagem do tamanho do registro
    */
    if ((*V).tamanhoRegistro == 0){

        // Armazena o último byte escrito e calcula o tamanho do registro
        long int end_byte = ftell(file_bin_w);
        (*V).tamanhoRegistro = end_byte - start_byte;

        // Retorna o ponteiro do arquivo nas posições reservadas ao tamanho e o atualiza 
        fseek(file_bin_w, start_byte - sizeof(int), SEEK_SET);
        fwrite(&(*V).tamanhoRegistro, sizeof(int), 1, file_bin_w);

        // Posiciona o ponteiro do arquivo ao final do registro escrito
        fseek(file_bin_w, 0, SEEK_END);

    }

    return 0;
}

/*
 * Lê um registro tipo 2 de um arquivo binário de dados pelo offset
*/
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

    /* 
     * Itera 3 vezes, referente aos 3 possíveis campos de 
     * tamanho variável de Vehicle
    */
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

        // Compara o aux_char para realizar a leitura correta de cada campo
        // de tamanho variável
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

/*
 * Remove um registro tipo 2 do arquivo de dados pelo offset
*/
int remove_reg_by_offset(FILE *file_bin_rw, long int *offset, Header *header){

    // Posicionando o ponteiro no registro a ser deletado
    fseek(file_bin_rw, (*offset), SEEK_SET);

    // Marcando como removido
    char is_removed;
    is_removed = '1';
    fwrite(&is_removed, sizeof(char), 1, file_bin_rw);

    int this_tamReg;
    fread(&this_tamReg, sizeof(int), 1, file_bin_rw);

    // next_offset recebe o topo da lista de registros removidos
    long int next_offset = header->topo.offset; 
    long int previous_offset = next_offset;
    int next_tamReg = -1;

    // Caso a lista não esteja vazia
    if (next_offset != -1){
        fseek(file_bin_rw, next_offset+1, SEEK_SET);
        fread(&next_tamReg, sizeof(int), 1, file_bin_rw);
    }

    /* 
     * Caso o registro removido seja maior do que o topo da lista
    */
    if (this_tamReg > next_tamReg){

        // Posicionando o ponteiro no campo do próximo offset 
        fseek(file_bin_rw, (*offset)+5, SEEK_SET);
        
        // Atualizando header
        previous_offset = next_offset;
        fwrite(&previous_offset, sizeof(long int), 1, file_bin_rw);

        header->topo.offset = (*offset);
        header->nroRegRem += 1;

        return 0;
    }

    /* 
     * Caso o registro removido seja menor do que o topo da lista
    */

    // previous_offset recebe next_offset e next_offset é atualizado com o valor
    // seguinte da lista
    previous_offset = next_offset;
    fread(&next_offset, sizeof(long int), 1, file_bin_rw);

    // Enquanto a lista não tiver atingido seu fim
    while(next_offset != -1){

        // Lê o tamanho do campo apontado por next_offset
        fseek(file_bin_rw, next_offset+1, SEEK_SET);
        fread(&next_tamReg, sizeof(int), 1, file_bin_rw);

        /* 
         * Caso o tamanho do registro a ser removido seja maior do que o tamanho
         * do apontado por next_offset, reaproveita seu espaço
        */
        if(this_tamReg > next_tamReg){

            // Registro removido passa a apontar para next
            fseek(file_bin_rw, (*offset)+5, SEEK_SET);
            fwrite(&next_offset, sizeof(long int), 1, file_bin_rw);

            // Registro anterior passa a apontar para registro removido
            fseek(file_bin_rw, previous_offset+5, SEEK_SET);
            fwrite(&(*offset), sizeof(long int), 1, file_bin_rw);

            // Header é atualizado
            header->nroRegRem += 1;
            return 0;
        }

        // Caso contrário, avança na lista
        previous_offset = next_offset;
        fread(&next_offset, sizeof(long int), 1, file_bin_rw);

    }

    /* 
     * Caso o final da lista seja alcançado
    */

    // Registro removido passa a apontar para next (que contém o valor -1)
    fseek(file_bin_rw, (*offset)+5, SEEK_SET); 
    fwrite(&next_offset, sizeof(long int), 1, file_bin_rw);

    // Registro anterior passa a apontar para registro removido
    fseek(file_bin_rw, previous_offset+5, SEEK_SET);
    fwrite(&(*offset), sizeof(long int), 1, file_bin_rw);

    // Header é atualizado
    header->nroRegRem += 1;

    return 0;
}

/*
 * Limpa um campo tipo 2, sobrescrevendo seus dados com '$'
*/
void clean_reg_type2(FILE *file_bin_rw, int tamReg){

    /*
     * Atenção! Esta função pressupõe que o ponteiro do arquivo
     * já esteja apontando para o ínicio do registro tipo 2
     * a ser limpo
    */

    // Armazena o offset atual
    long int cur_offset = ftell(file_bin_rw);

    // Preenche todo o tamanho do registro com '$'
    char aux = '$';
    for(int i=0; i < tamReg+5; i++){
        fwrite(&aux, sizeof(char), 1, file_bin_rw);
    }

    // Retorna o ponteiro à posição inicial
    fseek(file_bin_rw, cur_offset, SEEK_SET);

    return;
}

/*
 * Adiciona um novo registro tipo 2 ao arquivo de dados
*/
int add_new_reg_type2(FILE *file_bin_rw, Vehicle V, Header *header, long int *offset){

    int flag_list = 0; // flag para indicar se houve reaproveitamento de espaço

    // Checa se a lista de registros removidos não está vazia
    if (header->topo.offset == -1){
        // Caso esteja vazia, offset recebe o próximo offset disponível
        V.tamanhoRegistro = 0;
        (*offset) = header->prox.proxByteOffset;
    }
    else{ 
        // Caso não esteja vazia, offset recebe valor da lista e flag_stack é marcada 
        (*offset) = header->topo.offset;
        flag_list = 1;
    }

    // Posiciona ponteiro no offset onde novo registro será escrito
    fseek(file_bin_rw, (*offset), SEEK_SET);

    int tamReg = -1; 

    /*
     * Caso lista não esteja vazia, checa se seu espaço pode ser reaproveitado
    */
    if (flag_list){
        char is_removed; 
        long int new_list_top; 

        // Caso registro não conste como removido, retorna
        fread(&is_removed, sizeof(char), 1, file_bin_rw);
        if (is_removed != '1'){
            return -1;
        }

        // Lê tamanho do registro removido e compara com o registro novo
        fread(&tamReg, sizeof(int), 1, file_bin_rw);

        /*
         * Caso o registro novo tenha tamanho menor ou igual ao registro removido
         * seu espaço é reaproveitado para o novo registro
        */
        if (tamReg >= V.tamanhoRegistro){

            // Lê offset do valor ao qual o registro removido aponta
            fread(&new_list_top, sizeof(long int), 1, file_bin_rw);

            // Posiciona ponteiro no início do registro
            fseek(file_bin_rw, -(sizeof(char)+sizeof(long int)+sizeof(int)), SEEK_CUR);

            // Limpa registro para ser reaproveitado
            clean_reg_type2(file_bin_rw, tamReg);

            /*
             * V.tamanhoRegistro recebe o tamanho do registro removido. Isto é feito
             * para reaproveitar o espaço corretamente e reutilizar a função de
             * escrita de registros tipo 2
            */
            V.tamanhoRegistro = tamReg;

            // Atualizando o topo da pilha e o nroRegRem
            header->topo.offset = new_list_top;
            header->nroRegRem = header->nroRegRem - 1;
        }
        else{
            
            /*
             * Caso o registro novo seja maior do que o registro do topo da lista,
             * deve ser armazenado ao final do arquivo (consequência da ordenação 
             * por Worst Fit)
            */

            (*offset) = header->prox.proxByteOffset;
            fseek(file_bin_rw, (*offset), SEEK_SET);

            /* 
             * Tamanho é marcado como 0 para se reaproveitar corretamente a função 
             * de escrita de registros tipo 2
             * Como o espaço não está sendo reaproveitado, flag_list volta a ser 0
            */
            V.tamanhoRegistro = 0;
            flag_list = 0;
        }
    }

    // Escreve novo registro no offset atual
    write_reg_in_bin_type2(file_bin_rw, &V);

    // Caso não tenha tido reaproveitamento de espaço, próximo offset é atualizado
    if (!flag_list){
        long int prox_offset = ftell(file_bin_rw);
        header->prox.proxByteOffset = prox_offset;
    }

    return 0;
}

/*
 * Lê um id de um registro tipo 1 dado seu RRN
*/
int read_id_from_reg_type2(FILE *file_bin_r, int *id, long int *offset, Header *header){
    
    // Caso o arquivo de registros não esteja consistente, retorna
    if (header->status != '1'){
        return 1;
    }

    // Colocando o ponteiro do arquivo no registro a ser buscado
    fseek(file_bin_r, (*offset), SEEK_SET);

    // Caso não haja mais registros a serem lidos, retorna sinal de erro 3
    char is_removed;
    if (!fread(&is_removed, sizeof(char), 1, file_bin_r)){
        return 2;
    }

    // Caso o registro esteja removido, retorna 
    if (is_removed == '1'){
        (*id) = -1;

        // Atualizando offset
        int tam_registro;
        fread(&tam_registro, sizeof(int), 1, file_bin_r);
        (*offset) += tam_registro + sizeof(int) + sizeof(char);

        return 0;
    }

    // Atualizando offset
    int tam_registro;
    fread(&tam_registro, sizeof(int), 1, file_bin_r);
    (*offset) += tam_registro + sizeof(int) + sizeof(char);

    // Posicionando cursor no ID
    fseek(file_bin_r, sizeof(long int), SEEK_CUR);

    // Lê ID do registro indicado por rrn
    fread(&(*id), sizeof(int), 1, file_bin_r);

    return 0;
}

/*
 * Atualiza campo de registro tipo 2 identificado por seu offset
 * com dados presentes em Vehicle V
*/
int update_reg_type2(FILE *file_bin_rw, Vehicle V, Header *header, long int *offset, Index **I_list, int n_indices){

    // Checa se registro não está removido
    char is_removed;
    fread(&is_removed, sizeof(char), 1, file_bin_rw);
    if(is_removed == '1')
        return -1;

    // Lê o tamanho do registro antigo
    int tamReg;
    fread(&tamReg, sizeof(int), 1, file_bin_rw);

    // Caso o tamanho do registro atualizado seja maior do que o registro antigo
    if (tamReg < V.tamanhoRegistro){

        // Remove registro antigo e insere registro atualizado
        remove_reg_by_offset(file_bin_rw, offset, header);
        add_new_reg_type2(file_bin_rw, V, header, offset);

        // Atualiza lista de índices
        update_index(I_list, &n_indices, V.id, (*offset), TYPE);

        return 0;
    }

    // Limpa o registro antigo e escreve registro atualizado
    fseek(file_bin_rw, -(sizeof(char)+sizeof(int)), SEEK_CUR);
    clean_reg_type2(file_bin_rw, tamReg);
    write_reg_in_bin_type2(file_bin_rw, &V);

    return 0;
}
