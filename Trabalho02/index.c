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

struct idxHeader{
    char status;    // consistência do arquivo de índices
};

struct index{
    int id;         // id do registro no arquivo de índices
    union{
        int rrn;                // rrn do registro
        long int byteoffset;    // offset do registro
    }idx;
};


/*
 * OBSERVAÇÃO
 *  Foi optado por manter todas as operações de índices 
 *  em um único arquivo devido à quantidade pequena de 
 *  funções específicas de cada tipo de dados
*/

/*
 * Cria uma variável do tipo indice
*/
Index create_index(int f_type){
    
    Index I;

    I.id = -1;
    if (f_type == 1) I.idx.rrn = -1;
    else I.idx.byteoffset = -1;

    return I;
}

/*
 * Escreve cabeçalho de índices
*/
int write_idx_header(FILE *file_idx_w){

    char status = '0';
    fwrite(&status, sizeof(char), 1, file_idx_w);

    return 0;
}

/*
 * Carrega os dados do arquivo de dados em uma lista de índices na memória
*/
Index* load_all_idx_from_bin(FILE *file_bin_r, int f_type, int *n_indices, Header *header){
    
    // Aloca uma região para lista de índices
    Index *I_list = (Index *) malloc(BUFFER*sizeof(Index));
    Index I = create_index(f_type);

    int counter = 0;
    int id = -1;

    // Realiza diferentes rotinas a depender do tipo a ser lido
    if (f_type == 1){

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_id_from_reg_type1(file_bin_r, &id, counter, header)){

            if (id != -1){
                
                // Atribuindo valores a um índice
                I.id = id;
                I.idx.rrn = counter;

                // Caso o tamanho da lista seja alterado, aloca mais
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
        while(!read_id_from_reg_type2(file_bin_r, &id, &new_offset, header)){

            if (id != -1){
                
                // Atribuindo valores a um índice
                I.id = id;
                I.idx.byteoffset = offset;

                // Caso o tamanho da lista seja alterado, aloca mais
                if (counter*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                    I_list = (Index *) realloc(I_list, (counter / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));
                
                I_list[counter] = I;
            }
            counter++;
            offset = new_offset;
        }
    }

    // Adiciona um último elemento à lista com id == -1 para condição de parada
    I = create_index(f_type);
    I_list[counter] = I;

    // Realoca a lista para o tamanho necessário
    I_list = (Index *) realloc(I_list, (counter+1)*sizeof(Index));

    // Atualiza o número de índices
    (*n_indices) = counter;

    return I_list;
}

/* 
 * Escreve um índice tipo 1 no arquivo de índices
*/
int write_idx_in_bin_type1(FILE *file_idx_w, Index I){

    fwrite(&(I.id), sizeof(int), 1, file_idx_w);
    fwrite(&(I.idx.rrn), sizeof(int), 1, file_idx_w);

    return 0;
}

/* 
 * Escreve um índice tipo 2 no arquivo de índices
*/
int write_idx_in_bin_type2(FILE *file_idx_w, Index I){

    fwrite(&(I.id), sizeof(int), 1, file_idx_w);
    fwrite(&(I.idx.byteoffset), sizeof(long int), 1, file_idx_w);

    return 0;
}

/* 
 * Escreve um arquivo de índices baseado em um arquivo binário de dados
*/
int write_idx_file_from_bin(FILE *file_bin_r, Header *header, char *output_idx_filename, int f_type){

    FILE *file_idx_w = fopen(output_idx_filename, "wb");

    // Escreve cabeçalho de arquivo de índices
    write_idx_header(file_idx_w);

    // Carrega todos os índices em uma lista na memória para ordená-los
    // antes de efetuar a escrita
    int n_indices = 0;
    Index *I_list = load_all_idx_from_bin(file_bin_r, f_type, &n_indices, header);
    quick_sort(I_list, 0, n_indices-1);
    
    // Executa diferentes rotinas a depender do tipo
    if (f_type == 1){
        
        int i = 0;
        Index I = I_list[i]; 

        // Equanto não atinge condição de parada, escreve índices
        while(I.id != -1){

            write_idx_in_bin_type1(file_idx_w, I);

            I = I_list[++i]; 
        }
        
    }
    else if (f_type == 2){

        int i = 0;
        Index I = I_list[i]; 

        // Equanto não atinge condição de parada, escreve índices
        while(I.id != -1){
            write_idx_in_bin_type2(file_idx_w, I);
            I = I_list[++i]; 
        }
    }

    // Setando o status do índice para consistente
    set_status_file(file_idx_w,'1');

    // Fecha arquivos e libera memória
    fclose(file_idx_w);
    free(I_list);

    return 0;
}

/*
 * Lê um índice tipo 1 do arquivo de índices 
*/
int read_idx_type1(FILE *file_idx_r, Index *I){

    /* Atenção!
     * Esta função pressupõe que o ponteiro do arquivo já 
     * esteja posicionado corretamente
    */

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*I).id, sizeof(int), 1, file_idx_r)){
        return 1;
    }

    fread(&(*I).idx.rrn, sizeof(int), 1, file_idx_r);

    return 0;
}

/*
 * Lê um índice tipo 2 do arquivo de índices 
*/
int read_idx_type2(FILE *file_idx_r, Index *I){

    /* Atenção!
     * Esta função pressupõe que o ponteiro do arquivo já 
     * esteja posicionado corretamente
    */

    // Lê ID do registro indicado por rrn
    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*I).id, sizeof(int), 1, file_idx_r)){
        return 1;
    }

    fread(&(*I).idx.byteoffset, sizeof(long int), 1, file_idx_r);

    return 0;
}

/*
 * Imprime uma variável do tipo Index
*/
void print_index(Index I, int f_type){
    
    printf("Id: %d\n", I.id);
    if (f_type == 1) printf("RRN: %d\n", I.idx.rrn);
    else printf("Offset: %ld\n", I.idx.byteoffset);

    return;
}

/*
 * Troca índices passados por parâmetro
*/
void swap(Index *a, Index *b){
    Index aux = *a;
    *a = *b;
    *b = aux;

    return;
}

/*
 * Ordena através do método quick_sort
*/
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

/*
 * Carrega todos os índices do arquivo de índices em uma lista na memória RAM
*/
Index* load_all_indices_from_idx(FILE *file_idx_r, int f_type, int *n_indices){

    // Aloca uma região de memória para a lista de índices
    Index *I_list = (Index *) malloc(BUFFER*sizeof(Index));
    Index I = create_index(f_type);

    // Posiciona o ponteiro para ser os índices do arquivo
    fseek(file_idx_r, 1, SEEK_SET);

    // Realiza diferentes rotinas a depender do tipo a ser lido
    int counter = 0;
    if (f_type == 1){
        
        // Enquanto ainda há índices a serem lidos, continua
        while(!read_idx_type1(file_idx_r, &I)){

            // Caso o tamanho da lista seja alterado, aloca mais
            if (counter*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                I_list = (Index *) realloc(I_list, (counter / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));

            I_list[counter] = I;
            I = create_index(f_type);
            counter++;
        }

    }
    else if (f_type == 2){

        // Enquanto ainda há índices a serem lidos, continua
        while(!read_idx_type2(file_idx_r, &I)){

            // Caso o tamanho da lista seja alterado, aloca mais
            if (counter*sizeof(Index) % (BUFFER * sizeof(Index)) == 0) 
                I_list = (Index *) realloc(I_list, (counter / (BUFFER*sizeof(Index)) + 1) * BUFFER*sizeof(Index));

            I_list[counter] = I;
            I = create_index(f_type);
            counter++;
        }

    }

    (*n_indices) = counter;

    I_list = (Index *) realloc(I_list, counter*sizeof(Index));

    return I_list;
}

/*
 * Realiza uma busca binária por key em uma lista de índices I_list
*/
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

/*
 * Busca por um determinado índice key_id e retorna seu RRN ou offset
*/
long int search_index_from_idx(Index *I_list, int n_indices, int key_id, int f_type){

    int idx = binary_search_idx(I_list, key_id, 0, n_indices-1);

    // Caso não seja encontrado, retorna -1
    if (idx == -1) return -1;

    // Retorno depende do tipo do arquivo
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

/*
 * Adiciona um novo índice à lista de índices
*/
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

/*
 * Remove um índice da lista de índices
*/
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

/*
 * Atualiza o RRN ou offset de um índice específico na lista de índices
*/
int update_index(Index **I_list, int *n_indices, int id, long int new_rrn_byteoffset, int f_type){

    // Busca o índice na lista de índices do registro cujo RRN ou offset
    // será atualizado
    int idx = binary_search_idx((*I_list), id, 0, (*n_indices)-1);

    // Atualiza o valor do RRN ou do offset
    if (f_type == 1) (*I_list)[idx].idx.rrn = new_rrn_byteoffset;
    else if (f_type == 2) (*I_list)[idx].idx.byteoffset = new_rrn_byteoffset;

    return 0;
}

/*
 * Atualiza o arquivo de índices sobrescrevendo o antigo com os dados presentes em
 * I_list
*/
int refresh_idx_file(char *f_idx, Index *I_list, int n_indices, int f_type){

    FILE *file_idx_wr = fopen(f_idx, "wb+");
    
    // Escrevendo status como 'inconsistente' durante a escrita
    char status = '0';
    fwrite(&status, sizeof(char), 1, file_idx_wr);

    // Escrevendo os índices sequencialmente no arquivo
    // Diferentes rotinas a depender do tipo
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
    set_status_file(file_idx_wr,'1');

    fclose(file_idx_wr);

    return 0;
}

/*
 * Retorna o RRN de um dado índice em uma lista de índices I_list
*/
int get_rrn(Index *I_list, int *n_indices, int id){

    /* Atenção!
     * Esta função pressupõe que o registro seja
     * do tipo 1
    */

    // Busca o RRN da lista de índices e retorna
    int idx = binary_search_idx(I_list, id, 0, (*n_indices)-1);
    return I_list[idx].idx.rrn;
  
}

char get_idx_status(FILE *file_idx_r){

    /*
     * Para esta função, foi escolhido manter o offset atual
     * em vez de pressupor que o ponteiro esteja na posição
     * 0 do arquivo.
     * Esta escolha foi feita para modularizar melhor e para 
     * que a função possa ser utilizada em mais de um contexto
     * caso necessário
    */

    // Armazena offset atual
    long int cur_offset = ftell(file_idx_r);

    // Lê status do arquivo de índices
    char status;
    fseek(file_idx_r, 0, SEEK_SET);
    fread(&status, sizeof(char), 1, file_idx_r);

    // Retorna ao offset inicial
    fseek(file_idx_r, cur_offset, SEEK_SET);

    return status;
}
