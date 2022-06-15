#include <stdio.h>
#include <stdlib.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./index.h"
#include "./lista_encadeada.h"

#define MAX_RRN 97
#define REG_HEADER_SIZE_TYPE1 182
#define REG_HEADER_SIZE_TYPE2 189
#define BUFFER 4096

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

Index* load_all_idx_from_bin(FILE *file_bin_r, int f_type, int *n_indices){
    
    Index *I_list = (Index *) malloc(BUFFER*sizeof(Index));
    Index I = create_index(f_type);

    // Realiza diferentes rotinas a depender do tipo a ser lido
    if (f_type == 1){

        // Caractere auxiliar para verificar se o primeiro byte a ser lido se
        // refere a um registro. Retorna sinal de erro 1 caso não seja
        /*
        char c_aux;
        fread(&c_aux, sizeof(char), 1, file_bin_r);
        if (c_aux == '0'){
            fclose(file_bin_r);
            return 1;
        }
        fseek(file_bin_r,0,SEEK_SET);
        */

        int rrn = 0;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_id_from_reg_type1(file_bin_r, &I, rrn)){

            if (rrn*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                I_list = (Index *) realloc(I_list, (rrn / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));
            I_list[rrn] = I;
            rrn++;
        }

        I = create_index(f_type);
        I_list[rrn] = I;
        I_list = (Index *) realloc(I_list, (rrn+1)*sizeof(Index));

        (*n_indices) = rrn;
    }
//    PrintList(IndexList);

    return I_list;
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
    int n_indices = 0;
    Index *I_list = load_all_idx_from_bin(file_bin_r, f_type, &n_indices);
    //quick_sort(I_list, 0, n_indices);
    
    if (f_type == 1){
        
        int i = 0;
        Index I = I_list[i]; 

        while(I.id != -1){

            fwrite(&(I.id), sizeof(int), 1, file_idx_w);
            fwrite(&(I.idx.rrn), sizeof(int), 1, file_idx_w);

            //print_reg_from_bin_by_rrn("binario1_teste.bin", I.idx.rrn);
            //print_index(I, 1);
            I = I_list[++i]; 
        }
        
    }

    fclose(file_bin_r);
    fclose(file_idx_w);
    free(I_list);

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

    Index I = create_index(f_type);

    if (f_type == 1){
        
        fseek(file_idx_r, 1, SEEK_SET);

        int idx_rrn = 0; 
        while(!read_idx_type1(file_idx_r, &I, idx_rrn)){
            
            print_reg_from_bin_by_rrn("binario1_teste.bin", I.idx.rrn);

            I = create_index(f_type);
            idx_rrn++;
        }
        
    }
    
    fclose(file_idx_r);

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

void print_index(Index I, int f_type){
    
    printf("Id: %d\n", I.id);
    if (f_type == 1) printf("RRN: %d\n", I.idx.rrn);
    else printf("Offset: %ld\n", I.idx.byteoffset);

    return;
}

void swap(Index *a, Index *b){
    Index aux = *a;
    *a = *b;
    *b = aux;

    return;
}

void quick_sort(Index *I, int ini, int fim){

	int meio = (int) ((ini + fim) / 2);
	int m = I[meio].id;

	int i = ini, j = fim;
	do{
		while (I[i].id < m) i++;
		while (I[j].id > m) j--;

		if(i <= j){
			swap(&I[i],&I[j]);
			i++;
			j--;
		}

	} while(j > i);

	if(ini < j) quick_sort(I, ini, j);
	if(i < fim) quick_sort(I, i, fim);
}
