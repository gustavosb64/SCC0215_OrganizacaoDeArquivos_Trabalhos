#include <stdio.h>
#include <stdlib.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./index.h"

#define MAX_RRN 97
#define REG_HEADER_SIZE_TYPE1 182
#define REG_HEADER_SIZE_TYPE2 190
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

    char status = '0';
    fwrite(&status, sizeof(char), 1, file_idx_w);

    return 0;
}

/* AUTOTAD_PRIVATE
int read_id_from_reg_type1(FILE *file_bin_r, Index *I, int rrn){
    
    //Colocando o ponteiro do arquivo no ID do registro a ser buscado
    long int id_offset = MAX_RRN*rrn + REG_HEADER_SIZE_TYPE1 + sizeof(char)+sizeof(int);
    fseek(file_bin_r, id_offset, SEEK_SET);

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*I).id, sizeof(int), 1, file_bin_r)) 
        return 1;

    (*I).idx.rrn = rrn;

    return 0;
}
*/

Index* load_all_idx_from_bin(FILE *file_bin_r, int f_type, int *n_indices){
    
    Index *I_list = (Index *) malloc(BUFFER*sizeof(Index));
    Index I = create_index(f_type);

    int counter = 0;
    int id = -1;

    // Realiza diferentes rotinas a depender do tipo a ser lido
    if (f_type == 1){

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_id_from_reg_type1(file_bin_r, &id, counter)){

            if (id != -1){
                
                // Atribuindo valores a um índice
                I.id = id;
                I.idx.rrn = counter;

                if (counter*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                    I_list = (Index *) realloc(I_list, (counter / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));
                
                I_list[counter] = I;
            }

            counter++;
        }

    }

    else if (f_type == 2){

        long int new_offset = REG_HEADER_SIZE_TYPE2;
        long int offset = new_offset;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_id_from_reg_type2(file_bin_r, &id, &new_offset)){

            if (id != -1){
                
                // Atribuindo valores a um índice
                I.id = id;
                I.idx.byteoffset = offset;

                if (counter*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                    I_list = (Index *) realloc(I_list, (counter / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));
                
                I_list[counter] = I;
            }
            counter++;
            offset = new_offset;
        }
    }

    I = create_index(f_type);
    I_list[counter] = I;

    I_list = (Index *) realloc(I_list, (counter+1)*sizeof(Index));

    (*n_indices) = counter;

    return I_list;
}

int write_idx_in_bin_type1(FILE *file_idx_w, Index I){

    fwrite(&(I.id), sizeof(int), 1, file_idx_w);
    fwrite(&(I.idx.rrn), sizeof(int), 1, file_idx_w);

    return 0;
}

int write_idx_in_bin_type2(FILE *file_idx_w, Index I){

    fwrite(&(I.id), sizeof(int), 1, file_idx_w);
    fwrite(&(I.idx.byteoffset), sizeof(long int), 1, file_idx_w);

    return 0;
}

int write_idx_file_from_bin(char *input_bin_filename, char *output_idx_filename, int f_type){

    // Caso haja falha na leitura do arquivo, retorna 1
    FILE *file_bin_r = fopen(input_bin_filename, "rb");
    if (file_bin_r == NULL){
        return 1;
    }
    FILE *file_idx_w = fopen(output_idx_filename, "wb");

    write_idx_header(file_idx_w);

    // Carrega todos os índices em uma lista na memória para ordená-los
    // antes de efetuar a escrita
    int n_indices = 0;
    Index *I_list = load_all_idx_from_bin(file_bin_r, f_type, &n_indices);
    quick_sort(I_list, 0, n_indices-1);
    
    if (f_type == 1){
        
        int i = 0;
        Index I = I_list[i]; 

        while(I.id != -1){

            write_idx_in_bin_type1(file_idx_w, I);

            I = I_list[++i]; 
        }
        
    }
    else if (f_type == 2){

        int i = 0;
        Index I = I_list[i]; 

        while(I.id != -1){

            //print_index(I,2);
            write_idx_in_bin_type2(file_idx_w, I);

            I = I_list[++i]; 
        }
    }

    // Setando o status para consistente
    set_status_idx(file_idx_w,'1');

    fclose(file_bin_r);
    fclose(file_idx_w);
    free(I_list);

    return 0;
}

int read_idx_type1(FILE *file_idx_r, Index *I, int idx_counter){

    // Posicionando o ponteiro no índice a ser lido. Como o arquivo de índices
    // possui registros de tamanho fixo, podemos utilizar um idx_counter
    /*
    int size_index = sizeof(int) + sizeof(int);
    long int offset = sizeof(char) + idx_counter*size_index;
    fseek(file_idx_r, offset, SEEK_SET);
    */

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*I).id, sizeof(int), 1, file_idx_r)){
        return 1;
    }

    fread(&(*I).idx.rrn, sizeof(int), 1, file_idx_r);

    //printf("idx_counter: %d id: %d rrn: %d\n", idx_counter, (*I).id, (*I).idx.rrn);

    return 0;
}

int read_idx_type2(FILE *file_idx_r, Index *I, int idx_counter){

    // Posicionando o ponteiro no índice a ser lido. Como o arquivo de índices
    // possui registros de tamanho fixo, podemos utilizar um idx_counter
    int size_index = sizeof(int) + sizeof(long int);
    long int offset = sizeof(char) + idx_counter*size_index;
    fseek(file_idx_r, offset, SEEK_SET);

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*I).id, sizeof(int), 1, file_idx_r)){
        return 1;
    }

    fread(&(*I).idx.byteoffset, sizeof(long int), 1, file_idx_r);

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
            
            //print_reg_from_bin_by_rrn("binario1_teste.bin", I.idx.rrn);
            //print_index(I, 1);
            I = create_index(f_type);
            idx_rrn++;
        }
        
    }
    
    fclose(file_idx_r);

    return 0;
}

/* AUTOTAD_PRIVATE
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
*/

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

int set_status_idx(FILE *file_idx_rw, char status){

    if (status != '0' && status != '1')
        return -1;

    long int cur_offset = ftell(file_idx_rw);

    fseek(file_idx_rw, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file_idx_rw);

    fseek(file_idx_rw, cur_offset, SEEK_SET);

    return 0;
}

Index* load_all_indices_from_idx(FILE *file_idx_r, int f_type, int *n_indices){

    Index *I_list = (Index *) malloc(BUFFER*sizeof(Index));
    //Index *I_list = (Index *) malloc(10000000);
    Index I = create_index(f_type);

    int counter = 0;

    fseek(file_idx_r, 1, SEEK_SET);

    // Realiza diferentes rotinas a depender do tipo a ser lido
    if (f_type == 1){
        
        while(!read_idx_type1(file_idx_r, &I, counter)){

            /*
            print_index(I, 1);

            */
            if (counter*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                I_list = (Index *) realloc(I_list, (counter / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));

//            printf("%d\n",counter);
            I_list[counter] = I;
            I = create_index(f_type);
            counter++;
        }

    }

    else if (f_type == 2){

        while(!read_idx_type2(file_idx_r, &I, counter)){

            if (counter*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                I_list = (Index *) realloc(I_list, (counter / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));

            I_list[counter] = I;
            counter++;
        }

    }

    (*n_indices) = counter;

    I_list = (Index *) realloc(I_list, counter*sizeof(Index));

    return I_list;
}

int binary_search_idx(Index *I_list, int key, int ini, int fim) {
	
	// caso base 1
	if (ini > fim) return -1;	

	// calcula ponto central e verifica se key foi encontrada
	int c = (int)((ini+fim)/2);  

	// caso base 2
	if (I_list[c].id == key) return c;   

	if (key < I_list[c].id) 	      
		return binary_search_idx(I_list, key, ini, c-1); 
	if (key > I_list[c].id)  
		return binary_search_idx(I_list, key, c+1, fim);

    return -2;
}

long int search_index_from_idx(Index *I_list, int n_indices, int key_id, int f_type){

    int idx = binary_search_idx(I_list, key_id, 0, n_indices-1);
    if (idx == -1){
        return -1;
    }

    if (f_type == 1){

        int rrn = I_list[idx].idx.rrn;

        return rrn;
    }
    else if (f_type == 2){

        long int offset = I_list[idx].idx.byteoffset;

        return offset;
    }

    return -2;
}

int add_new_index(Index **I_list, int *n_indices, int id, long int new_rrn_byteoffset, int f_type){

    // Cria um novo Index com os dados fornecidos
    Index I_new = create_index(1);
    I_new.id = id;
    if (f_type == 1) I_new.idx.rrn = new_rrn_byteoffset;
    else I_new.idx.byteoffset = new_rrn_byteoffset;

    // Adiciona o índice novo à lista
    (*n_indices) += 1;
    (*I_list) = (Index *) realloc((*I_list), sizeof(Index) * (*n_indices) );
    (*I_list)[(*n_indices)-1] = I_new;

    // Reordena os índices
    quick_sort((*I_list), 0, (*n_indices)-1);

    return 0;
}

int remove_index(Index **I_list, int *n_indices, int id){

    int idx = binary_search_idx((*I_list), id, 0, (*n_indices)-1);

    // Atribui um valor maior que o último da lista ao registro a ser removido
    (*I_list)[idx].id = (*I_list)[(*n_indices)-1].id + 1;

    // Reordena os índices, posicionando o elemento a ser removido em última posição
    quick_sort((*I_list), 0, (*n_indices)-1);

    // Realoca a lista, retirando o último elemento, 
    (*n_indices) -= 1;
    (*I_list) = (Index *) realloc((*I_list), sizeof(Index) * (*n_indices) );

    return 0;
}

int update_index(Index **I_list, int *n_indices, int id, long int new_rrn_byteoffset, int f_type){

    int idx = binary_search_idx((*I_list), id, 0, (*n_indices)-1);
    if (f_type == 1) (*I_list)[idx].idx.rrn = new_rrn_byteoffset;
    else if (f_type == 2) (*I_list)[idx].idx.byteoffset = new_rrn_byteoffset;

    return 0;
}

int refresh_idx_file(char *f_idx, Index *I_list, int n_indices, int f_type){

    FILE *file_idx_wr = fopen(f_idx, "wb+");
    
    // Escrevendo status como 'inconsistente' durante a escrita
    char status = '0';
    fwrite(&status, sizeof(char), 1, file_idx_wr);

    // Escrevendo os índices sequencialmente no arquivo
    if (f_type == 1){

        for(int i=0; i<n_indices; i++){
            fwrite(&(I_list[i].id), sizeof(int), 1, file_idx_wr);
            fwrite(&(I_list[i].idx.rrn), sizeof(int), 1, file_idx_wr);
        }

    }
    else if (f_type == 2){

        for(int i=0; i<n_indices; i++){
            fwrite(&(I_list[i].id), sizeof(int), 1, file_idx_wr);
            fwrite(&(I_list[i].idx.byteoffset), sizeof(long int), 1, file_idx_wr);
        }

    }
    else 
        return -1;

    // Setando o status do arquivo para consistente após a escrita
    set_status_idx(file_idx_wr,'1');

    fclose(file_idx_wr);

    return 0;
}
