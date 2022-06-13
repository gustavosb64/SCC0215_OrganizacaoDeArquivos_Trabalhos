#include <stdio.h>
#include <stdlib.h>
#include "./records.h"
#include "./index.h"
#include "./lista_encadeada.h"

#define MAX_RRN 97
#define REG_HEADER_SIZE_TYPE1 182
#define REG_HEADER_SIZE_TYPE2 189

struct idxHeader{
    char status;
};

struct index{
    int id;
    union{
        int rrn;
        long int byteoffset;
    }idx;
};

Index create_index(int f_type){
    
    Index I;

    I.id = -1;
    if (f_type == 1) I.idx.rrn = -1;
    else I.idx.byteoffset = -1;

    return I;
}

int write_idx_header(FILE *file_idx_w){

    char status = '1';
    fwrite(&status, sizeof(char), 1, file_idx_w);

    return 0;
}

int read_id_from_reg_type1(FILE *file_bin_r, Index *I, int rrn){
    
    // Colocando o ponteiro do arquivo no ID do registro a ser buscado
    long int id_offset = MAX_RRN*rrn + REG_HEADER_SIZE_TYPE1 + sizeof(char)+sizeof(int);
    fseek(file_bin_r, id_offset, SEEK_SET);

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*I).id, sizeof(int), 1, file_bin_r)) 
        return 1;

    (*I).idx.rrn = rrn;

    return 0;
}

int write_idx_file_from_bin(char *input_filename, char *output_filename, int f_type){

    // Caso haja falha na leitura do arquivo, retorna 1
    FILE *file_bin_r = fopen(input_filename, "rb");
    if (file_bin_r == NULL){
        return 1;
    }
    FILE *file_idx_w = fopen(output_filename, "wb");

    write_idx_header(file_idx_w);

    // Carrega todos os índices em uma lista na memória para ordená-los
    // antes de efetuar a escrita
    List *IndexList = load_all_idx_from_bin(file_bin_r, f_type);
    sort_by_merge(IndexList);

    Index I = create_index(f_type);

    if (f_type == 1){

        Node *I_node = GetFirstNode(IndexList);

        while(I_node != NULL){

            I = GetElem(I_node);

            fwrite(&(I.id), sizeof(int), 1, file_idx_w);
            fwrite(&(I.idx.rrn), sizeof(int), 1, file_idx_w);

            I_node = GetNextNode(I_node);
        }

    }

    fclose(file_bin_r);
    fclose(file_idx_w);
    FreeList(IndexList);

    return 0;
}

int read_idx_type1(FILE *file_idx_r, Index *I, int idx_rrn){

    // Posicionando o ponteiro no índice a ser lido. Como o arquivo de índices
    // possui registros de tamanho fixo, podemos utilizar um idx_rrn
    int size_index = 2*sizeof(int);
    long int offset = sizeof(char) + idx_rrn*size_index;
    fseek(file_idx_r, offset, SEEK_SET);

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*I).id, sizeof(int), 1, file_idx_r)){
        return 1;
    }

    fread(&(*I).idx.rrn, sizeof(int), 1, file_idx_r);

    return 0;
}

int read_all_indices_from_idx(char *input_filename, int f_type){

    FILE *file_idx_r = fopen(input_filename, "rb");
    if (file_idx_r == NULL){
        return 1;
    }

    List *IndexList = CreateList();
    Index I = create_index(f_type);

    if (f_type == 1){
        
        fseek(file_idx_r, 1, SEEK_SET);

        int idx_rrn = 0; 
        while(!read_idx_type1(file_idx_r, &I, idx_rrn)){
            
            AddLastElemList(IndexList, I);

            print_reg_from_bin_by_rrn("binario1_teste.bin", I.idx.rrn);

            I = create_index(f_type);
            idx_rrn++;
        }
        
    }
    
    fclose(file_idx_r);
    FreeList(IndexList);

    return 0;
}

int search_index_from_idx(char *input_filename, int src_id, int f_type){

    FILE *file_idx_r = fopen(input_filename, "rb");
    if (file_idx_r == NULL){
        return -2;
    }

    Index I = create_index(f_type);

    if (f_type == 1){

        fseek(file_idx_r, 1, SEEK_SET);

        int idx_rrn = 0; 
        while(!read_idx_type1(file_idx_r, &I, idx_rrn)){

            if (I.id == src_id) return I.idx.rrn;
            idx_rrn++;
        }

    }

    fclose(file_idx_r);

    return -1;
}
