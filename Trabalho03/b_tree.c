#include <stdio.h>
#include <stdlib.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./b_tree.h"

#define MAX_RRN 97
#define REG_HEADER_SIZE_TYPE1 182
#define REG_HEADER_SIZE_TYPE2 190
#define BUFFER 4096
#define NODE_SIZE_TYPE1 45
#define NODE_SIZE_TYPE2 57
#define MAX_NUM_KEYS 3

struct b_Header{
    char status;    // consistência do arquivo
    int noRaiz;     // RRN do nó raiz 
    int proxRRN;    // próximo RRN disponível para conter um nó
    int nroNos;     // quantidade de nós presentes na árvore
};

struct key{
    int C;  // Chave
    union{
        int rrn;            // RRN do arquivo de dados referente à chave
        long int offset;    // Offset do arquivo de dados referente à chave
    }Pr;    // Referência ao arquivo de dados
};

/* Type1: 45 bytes
 * Type2: 56 bytes */
struct node{
    char tipoNo;    // tipo do nó; 0 = raiz, 1 = intermediário, 2 = folha
    int nroChaves;  // número de chaves presentes no nó
    struct key key[4];     // Chaves e RRN/offset no arquivo de dados
    int P[5];       // Subárvores no arquivo da Árvore-B
};


/*
 * Inicializa um cabeçalho de Árvore-B
*/
B_Header* initialize_btree_header(){

    B_Header *b_header = (B_Header*) malloc(sizeof(B_Header));

    b_header->status = 0;
    b_header->noRaiz = -1;
    b_header->proxRRN = 0;
    b_header->nroNos = 0;

    return b_header;
}

int write_btree_header(FILE *file_btree_rw, B_Header *b_header, int f_type){

    // Seta status do cabeçalho para 1
    b_header->status = '1';

    // Escreve dados
    fseek(file_btree_rw, 0, SEEK_SET);

    fwrite(&(b_header->status), sizeof(char), 1, file_btree_rw);
    fwrite(&(b_header->noRaiz), sizeof(int), 1, file_btree_rw);
    fwrite(&(b_header->proxRRN), sizeof(int), 1, file_btree_rw);
    fwrite(&(b_header->nroNos), sizeof(int), 1, file_btree_rw);

    // Preenche o espaço remanescente com lixo
    char aux_char = '$';
    int len;

    if (f_type == 1) len = NODE_SIZE_TYPE1 - 13;
    else len = NODE_SIZE_TYPE2 - 13;

    for (int i=0; i<len; i++){
        fwrite(&(aux_char), sizeof(char), 1, file_btree_rw);
    }

    return 0;
}

/* 
 * Inicializa uma variável do tipo Node com valores padrão
*/
Node* initialize_node(int f_type){

    Node *node = (Node*) malloc(sizeof(Node));

    node->tipoNo = '2';     // iniciando o nó como uma folha
    node->nroChaves = 0;

    for(int i=0; i<4; i++){

        node->key[i].C = -1;
        if (f_type == 1) node->key[i].Pr.rrn = -1;
        else node->key[i].Pr.offset = -1;

        node->P[i] = -1;
    }
    node->P[4] = -1;

    return node;
}

int write_b_tree_header(FILE *file_btree_w, B_Header *b_header){

    fseek(file_btree_w, 0, SEEK_SET);

    fwrite(&b_header->status, sizeof(char), 1, file_btree_w);
    fwrite(&b_header->noRaiz, sizeof(int), 1, file_btree_w);
    fwrite(&b_header->proxRRN, sizeof(int), 1, file_btree_w);
    fwrite(&b_header->nroNos, sizeof(int), 1, file_btree_w);

    return 0;
}

B_Header* read_header_from_btree(FILE *file_btree_r){

    // Posiciona ponteiro no início do arquivo da Árvore-B
    fseek(file_btree_r, 0, SEEK_SET);

    // Lê dados do cabeçalho
    B_Header *b_header = (B_Header*) malloc(sizeof(B_Header));

    fread(&(b_header->status), sizeof(char), 1, file_btree_r);
    fread(&(b_header->noRaiz), sizeof(int), 1, file_btree_r);
    fread(&(b_header->proxRRN), sizeof(int), 1, file_btree_r);
    fread(&(b_header->nroNos), sizeof(int), 1, file_btree_r);

    return b_header;
}

/*
 * Lê um nó da Árvore-B dado seu RRN
*/
Node* read_node_from_b_tree(FILE *file_btree_r, int rrn_b_tree, int f_type){

    // Caso o RRN passado seja -1, retorna NULL
    if (rrn_b_tree == -1){
        return NULL;
    }

    // Posicionando ponteiro no início do nó a ser lido
    long int offset = 0;
    if (f_type == 1){
        offset = NODE_SIZE_TYPE1;
    }
    else{
        offset = NODE_SIZE_TYPE2;
    }
    offset += offset*rrn_b_tree;
    fseek(file_btree_r, offset, SEEK_SET);

    // Aloca região para nó e lê dados
    Node *node = initialize_node(f_type);

    fread(&(node->tipoNo), sizeof(char), 1, file_btree_r);
    fread(&(node->nroChaves), sizeof(int), 1, file_btree_r);

    // Lê chaves de referência
    for(int i=0; i < 3; i++){
        fread(&(node->key[i].C), sizeof(int), 1, file_btree_r);
        if (f_type == 1) fread(&(node->key[i].Pr.rrn), sizeof(int), 1, file_btree_r);
        else fread(&(node->key[i].Pr.offset), sizeof(long int), 1, file_btree_r);
    }
    
    // Lê referência para outros nós da árvore
    for(int i=0; i < 4; i++){
        fread(&(node->P[i]), sizeof(int), 1, file_btree_r);
    }

    return node;
}

long int search_in_page_b_tree(FILE *file_btree_r, Node *cur_node, int src_id, int f_type){

    /*
    //print_node(cur_node, 1);
    //printf("-------------\n");
    */

    // Itera por cada uma das chaves comparando com o ID buscado
    for (int i=0; i < cur_node->nroChaves; i++){
        ////printf("i: %d, src_id: %d, cur_node[C]: %d\n",i, src_id, cur_node->C[i]);

        // Caso a chave atual seja igual ao ID buscado, retorna sua referência
        if (cur_node->key[i].C == src_id){

            long int Pr;
            if (f_type == 1) Pr = cur_node->key[i].Pr.rrn;
            else Pr = cur_node->key[i].Pr.offset;

            free(cur_node);

            return Pr;
        }

        if (cur_node->key[i].C > src_id){
            
            /*
            //printf("Maior:\n");
            */
            
            Node *new_node = read_node_from_b_tree(file_btree_r, cur_node->P[i], f_type);
            free(cur_node);
            if (new_node == NULL){
                free(new_node);
                return -1;
            }

            /*
            //print_node(new_node, f_type);
            //printf("-------------\n");
            */
            
            return search_in_page_b_tree(file_btree_r, new_node, src_id, f_type);
        }

    }

    Node *new_node = read_node_from_b_tree(file_btree_r, cur_node->P[cur_node->nroChaves], f_type);
    free(cur_node);
    if (new_node == NULL){
        free(new_node);
        return -1;
    }

    return search_in_page_b_tree(file_btree_r, new_node, src_id, f_type);
}

/*
 * Busca índice src_id em Árvore-B 
*/
long int search_index_in_b_tree(FILE *file_bin_r, FILE *file_btree_r, int src_id, B_Header *b_header, Header *f_header, int f_type){

    
    /*
    //printf("noRaiz: %d\n", b_header->noRaiz);
    //printf("src_id: %d\n", src_id);
    //printf("\n");
    */

    // Declarando valor de retorno (i.e., índice no arquivo de dados)
    long int ref = -1;

    // Busca referência na Árvore-B
    Node *node = read_node_from_b_tree(file_btree_r, b_header->noRaiz, f_type);
    ref = search_in_page_b_tree(file_btree_r, node, src_id, f_type);

    return ref;
}

/* AUTOTAD_PRIVATE
Key* insertion_sort(Key* v, int N) {

	int i = 1;

	// Para cada elemento na lista desordenada
	while (i < N) {		
		int el = v[i].C;  // fixa elemento a ser comparado com anteriores
		int j = i-1;    // indice para percorrer elementos anteriores

		// enquanto houver elementos a serem comparados e o
		// elemento for menor do que o anterior
		while (j >= 0 && el < v[j].C) {
			v[j+1] = v[j];  // movimenta valor para frente
			j--;            // indice retorna uma posicao
		}

		// Encontrei a posicao correta
		v[j+1].C = el;

		i++;
	}

    return v;
}
*/

/*
 * Checa se nó node se trata de um nó folha
*/
int isLeafNode(Node *node){

    /*
    if (node->tipoNo == '1') return 0;
    if (node->tipoNo == '2') return 1;
    */

    if (node->P[0] != -1) return 0;
    else return 1;
}

/* 
 * Escreve um nó no arquivo de índices da Árvore-B
*/
int write_node_in_btree_file(FILE *file_btree_rw, Node *node, int rrn, int f_type){

    // Posicionando ponteiro no local de escrita
    int node_size;
    if (f_type == 1) node_size = NODE_SIZE_TYPE1;
    else node_size = NODE_SIZE_TYPE2;

    long int offset = (rrn*node_size) + node_size;
    fseek(file_btree_rw, offset, SEEK_SET);

    // Escrevendo dados
    fwrite(&node->tipoNo, sizeof(char), 1, file_btree_rw);
    fwrite(&node->nroChaves, sizeof(int), 1, file_btree_rw);

    for (int i=0; i<MAX_NUM_KEYS; i++){
        fwrite(&node->key[i].C, sizeof(int), 1, file_btree_rw);
        if (f_type == 1) fwrite(&node->key[i].Pr.rrn, sizeof(int), 1, file_btree_rw);
        else fwrite(&node->key[i].Pr.offset, sizeof(long int), 1, file_btree_rw);
    }

    for (int i=0; i<4; i++){
        fwrite(&node->P[i], sizeof(int), 1, file_btree_rw);
    }

    return 0;
}

void test_write_node(){
    
    FILE *file = fopen("indice_teste.bin", "rw+");
    if(file == NULL){
        //printf("oops\n");
    }
    B_Header *b_header = read_header_from_btree(file);

    Node *node = initialize_node(2);

    node->tipoNo = '2';
    node->key[0].C = 9;
    node->key[0].Pr.offset = 8;
    node->key[1].C = 32;
    node->key[1].Pr.offset = 7;

    node->nroChaves = 2;

    node->P[0] = 10;
    node->P[1] = 12;
    node->P[2] = 15;

    //printf("proxRRN: %d\n", b_header->proxRRN);
    //print_node(node, 2);

    write_node_in_btree_file(file, node, b_header->proxRRN, 2);
    fflush(file);

    fclose(file);

}

/*
 * Função de split do nó quando atinge a capacidade máxima. 
 *  Retorna 0 quando não ocorre split
 *  Retorna 1 quando ocorre split
*/
int split_node(FILE *file_btree_rw, Node *cur_node, B_Header *b_header, int f_type, Key *promo_key, int *promo_r_child){ 

    // Split ocorre quando o número de chaves for maior que o número máximo permitido
    if (cur_node->nroChaves <= MAX_NUM_KEYS){
        return 0;
    }

    //printf("@@@@@@@@@@@@@@@@\n");
    //printf("SPLIT\n");
    //printf("@@@@@@@@@@@@@@@@\n");
    //print_node(cur_node,2);
    //printf("----------------\n");

    // Cria um novo nó e o atualiza com valores 
    Node *new_node = initialize_node(f_type);
    //print_node(new_node,2);

    new_node->key[0] = cur_node->key[3];
    new_node->nroChaves = 1;
    new_node->P[0] = cur_node->P[3];
    new_node->P[1] = cur_node->P[4];

    // Terceira chave é promovida
    (*promo_key) = cur_node->key[2];
    (*promo_r_child) = b_header->proxRRN;

    // Escreve o novo nó ao final do arquivo
    write_node_in_btree_file(file_btree_rw, new_node, b_header->proxRRN, f_type);
    fflush(file_btree_rw);

    // Terceira e quarta chaves são removidas do nó 
    cur_node->key[2].C = -1;
    cur_node->key[3].C = -1;
    if (f_type == 1){
        cur_node->key[2].Pr.rrn = -1;
        cur_node->key[3].Pr.rrn = -1;
    }
    else{
        cur_node->key[2].Pr.offset = -1;
        cur_node->key[3].Pr.offset = -1;
    }
    cur_node->P[3] = -1;
    cur_node->P[4] = -1;
    cur_node->nroChaves = 2;

    // Atualiza header
    b_header->proxRRN += 1;
    b_header->nroNos += 1;

    return 1;
}

/* AUTOTAD_PRIVATE
int insert_new_id_in_node(FILE *file_btree_rw, Node *node, B_Header *b_header, Key new_key, int f_type, int *promo_id, int *promo_r_child){ 
}
*/

/*
 * Insere um novo id chave na página referenciada por node
*/
int insert_new_id_in_node(Node *node, Key new_key){ 

    //printf("################\n");
    //printf("INSERTING NEW ID\n");
    //printf("################\n");
    //print_node(node,2);
    //printf("new_key: %d\n",new_key.C);
    //printf("################\n");


    // Insere nova chave na página
    int new_idx = node->nroChaves;
    node->key[new_idx] = new_key;
    node->nroChaves += 1;
    
    // Ordena página com algoritmo insertion sort
    Key aux_key;
    for (int i=0; i < node->nroChaves; i++){

        int v = node->key[i].C;
        aux_key = node->key[i];
          
        int j;
        for (j = i-1; j >= 0; j--){
            if (node->key[j].C <= v) break;
            node->key[j+1] = node->key[j];
        }
        node->key[j+1] = aux_key;        
    }

    //print_node(node,2);
    //printf("################\n");

    return 0;
}

void test_insertion_sort(){

    Node *node = initialize_node(1);

    node->tipoNo = '2';
    node->nroChaves = 2;

    node->key[0].C = 4;
    node->key[0].Pr.rrn = 3;
    node->key[1].C = 8;
    node->key[1].Pr.rrn = 7;

    Key new_key;
    new_key.C = 6;
    new_key.Pr.rrn = 5;

    //print_node(node,2);
    //printf("_--------------\n");

    insert_new_id_in_node(node, new_key);

    //print_node(node,2);
    //printf("_--------------\n");

    new_key.C = 5;
    new_key.Pr.rrn = 80;

    insert_new_id_in_node(node, new_key);

    //print_node(node,2);
    //printf("_--------------\n");
}

/* 
 * Insere um novo RRN de subárvore em um nó especificado por node
*/
int insert_subtree_rrn_in_node(Node *node, int subtree_rrn, int index){

    int prev_p, next_p;
    next_p = subtree_rrn;
    for (int i=index; i < 5; i++){
        prev_p = next_p;
        next_p = node->P[i];
        node->P[i] = prev_p;
    }

    return 0;
}

int insert_btree(FILE *file_btree_rw, B_Header *b_header, Key new_key, int cur_rrn_btree, int f_type, Key *promo_key, int *promo_r_child, int recursion_counter){

    if (cur_rrn_btree == -1){
        return -2;
    }

    // Carrga página atual na memória
    Node *cur_node = read_node_from_b_tree(file_btree_rw, cur_rrn_btree, f_type);

    //printf("------------------\n");
    //print_node(cur_node, f_type);
    //printf("------------------\n");

    int nro_chaves = cur_node->nroChaves;
    int split_return = 0;
    for (int i=0; i < nro_chaves; i++){

        // Caso chave já exista, retorna sinal -1
        if (new_key.C == cur_node->key[i].C){
            free(cur_node);
            return -1;
        }

        // Caso o novo ID seja menor do que a chave atual, posição encontrada
        if (new_key.C < cur_node->key[i].C){
            //printf(">>>>>>>>>> 1.1\n");

            // Caso não seja um nó folha, afunda na árvore
            if (!isLeafNode(cur_node)){
                //printf(">>>>>>>>>> 1.2\n");

                // Novo nó a ser investigado
                int next_node_rrn = cur_node->P[i];
                split_return = insert_btree(file_btree_rw, b_header, new_key, next_node_rrn, f_type, promo_key, promo_r_child, recursion_counter+1);

                // Caso tenha ocorrido split no nó abaixo
                if(split_return == 1){
                    
                    //printf(">>>>>>>>>> 1.2.split\n");
                    // Insere a chave promovida no nó atual
                    insert_new_id_in_node(cur_node, (*promo_key));
                    
                    // Insere a referência da chava promovida
                    insert_subtree_rrn_in_node(cur_node, (*promo_r_child), i+1);

                    // Realiza split do nó (quando necessário)
                    split_return = split_node(file_btree_rw, cur_node, b_header, f_type, promo_key, promo_r_child);
                    
                    /* Caso tenha ocorrido split na primeira chamada, houve split 
                     * em um nó raiz; portanto, seu tipo é atualizando antes de 
                     * ser reescrito no arquivo*/
                    if (split_return == 1 && recursion_counter == 1){
                        cur_node->tipoNo = '1'; // como é um nó com filhos, passa a ser intermediário
                    }

                    // Atualiza nó no arquivo
                    write_node_in_btree_file(file_btree_rw, cur_node, cur_rrn_btree, f_type);
                    fflush(file_btree_rw);

                    free(cur_node);
                    return split_return;
                }

            }
            else{
                
                //printf(">>>>>>>>>> 1.3\n");
                // Insere o novo ID no nó atual
                insert_new_id_in_node(cur_node, new_key);

                // Realiza split do nó (quando necessário)
                split_return = split_node(file_btree_rw, cur_node, b_header, f_type, promo_key, promo_r_child);

                /* Caso tenha ocorrido split na primeira chamada, houve split 
                 * em um nó raiz; portanto, seu tipo é atualizando antes de 
                 * ser reescrito no arquivo*/
                if (split_return == 1 && recursion_counter == 1){
                    cur_node->tipoNo = '2'; // como é um nó sem filhos, passa a ser folha
                }

                // Atualiza nó no arquivo
                write_node_in_btree_file(file_btree_rw, cur_node, cur_rrn_btree, f_type);
                fflush(file_btree_rw);

                free(cur_node);
                return split_return;
            }

            free(cur_node);
            return 0;
        }

    }

    int i = cur_node->nroChaves;
    // Caso o novo ID seja maior do que a chave atual, posição encontrada
    if (new_key.C > cur_node->key[cur_node->nroChaves-1].C){
        
        //printf(">>>>>>>>>> 2.1\n");
        // Caso não seja um nó folha, afunda na árvore
        if (!isLeafNode(cur_node)){
            //printf(">>>>>>>>>> 2.2\n");

            // Novo nó a ser investigado
            int next_node_rrn = cur_node->P[cur_node->nroChaves];

            split_return = insert_btree(file_btree_rw, b_header, new_key, next_node_rrn, f_type, promo_key, promo_r_child, recursion_counter+1);

            // Caso tenha ocorrido split no nó abaixo
            if(split_return == 1){
                
                //printf(">>>>>>>>>> 2.2.split\n");
                // Insere a chave promovida no nó atual
                insert_new_id_in_node(cur_node, (*promo_key));
                
                // Insere a referência da chava promovida
                insert_subtree_rrn_in_node(cur_node, (*promo_r_child), i+1);

                // Realiza split do nó (quando necessário)
                split_return = split_node(file_btree_rw, cur_node, b_header, f_type, promo_key, promo_r_child);

                /* Caso tenha ocorrido split na primeira chamada, houve split 
                 * em um nó raiz; portanto, seu tipo é atualizando antes de 
                 * ser reescrito no arquivo*/
                if (split_return == 1 && recursion_counter == 1){
                    cur_node->tipoNo = '1'; // como é um nó com filhos, passa a ser intermediário
                }
                
                // Atualiza nó no arquivo
                write_node_in_btree_file(file_btree_rw, cur_node, cur_rrn_btree, f_type);
                fflush(file_btree_rw);
            }
        }
        else{

            //printf(">>>>>>>>>> 2.3\n");
            // Insere o novo ID no nó atual
            insert_new_id_in_node(cur_node, new_key);

            // Realiza split do nó (quando necessário)
            split_return = split_node(file_btree_rw, cur_node, b_header, f_type, promo_key, promo_r_child);

            /* Caso tenha ocorrido split na primeira chamada, houve split 
             * em um nó raiz; portanto, seu tipo é atualizando antes de 
             * ser reescrito no arquivo*/
            if (split_return == 1 && recursion_counter == 1){
                cur_node->tipoNo = '2'; // como é um nó sem filhos, passa a ser folha
            }

            // Atualiza nó no arquivo
            write_node_in_btree_file(file_btree_rw, cur_node, cur_rrn_btree, f_type);
            fflush(file_btree_rw);

            free(cur_node);
            return split_return;
        }

    }

    //printf(">>>>>>>>>> 3.0\n");

    free(cur_node);
    return split_return;
}

/*
 * Cria um novo nó raiz quando a árvore está vazia
*/
int initialize_btree(FILE *file_btree_rw, B_Header *b_header, Key new_key, int f_type){

    Node *new_root = initialize_node(f_type);
    
    // Insere a chave no nó raiz recém criado
    insert_new_id_in_node(new_root, new_key);
    new_root->tipoNo = '0';

    // Escreve novo nó no arquivo
    write_node_in_btree_file(file_btree_rw, new_root, b_header->proxRRN, f_type);
    fflush(file_btree_rw);
    
    free(new_root);
    return 0;
}

/*
 * Realiza rotinas de inserção de um novo nó em uma Árvore-B
*/
int add_new_node_btree(FILE *file_btree_rw, B_Header *b_header, int id, long int id_ref, int f_type){
    
    // Criando nova chave com os dados fornecidos
    Key new_key;
    new_key.C = id;
    if (f_type == 1) new_key.Pr.rrn = id_ref;
    else new_key.Pr.offset = id_ref;

    if (b_header->noRaiz == -1){
        initialize_btree(file_btree_rw, b_header, new_key, f_type);        
        return 0;
    }

    Key *promo_key = (Key*) malloc(sizeof(Key));
    int promo_r_child = -1;
    int recursion_counter = 0;

    int insertion_return = insert_btree(file_btree_rw, b_header, new_key, b_header->noRaiz, f_type, promo_key, &promo_r_child, recursion_counter+1);

    // Caso tenha ocorrido split no nó abaixo
    if(insertion_return == 1){

        //printf(">>>>>>>>>> 4.1.split <<<<<<<<<<\n");

        Node *new_root = initialize_node(f_type);

        // Insere a chave promovida no nó atual
        insert_new_id_in_node(new_root, (*promo_key));
        
        // Insere novos nós
        insert_subtree_rrn_in_node(new_root, b_header->noRaiz, 0);
        insert_subtree_rrn_in_node(new_root, promo_r_child, 1);

        // Atualiza nó no arquivo
        write_node_in_btree_file(file_btree_rw, new_root, b_header->proxRRN, f_type);
        fflush(file_btree_rw);

        // Atualiza cabeçalho com nova raiz
        b_header->noRaiz = b_header->proxRRN;
        b_header->proxRRN += 1;
    }

    //printf("split: %d\n",split_return);

    //printf("++++++++++++++++++++++++++++++\n");
    //printf("++++++++++++++++++++++++++++++\n");
    //printf("++++++++++++++++++++++++++++++\n");
    //printf("++++++++++++++++++++++++++++++\n");
    //printf("++++++++++++++++++++++++++++++\n");

    return 0;
}

int write_btree_file_from_bin(FILE *file_bin_r, Header f_header, char *btree_filename, int f_type){

    FILE *file_btree_w = fopen(btree_filename, "wb");
    if (file_btree_w == NULL){
        return 1;
    }

    Key new_key;
    if (f_type == 1){

        int rrn = 0;
        int id = -1;
        int counter = 0;

        while(!read_id_from_reg_type1(file_bin_r, &id, counter, f_header)){
            
            new_key.C = id; 
            new_key.Pr.rrn = counter; 

            if (id != -1){
            }
        }


    }

    return 0;
}

/* 
 * Imprime os valores de uma estrutura do tipo Node
*/
void print_node(Node *node, int f_type){
    
    printf("tipoNo: %c\n", node->tipoNo);
    printf("nroChaves: %d\n", node->nroChaves);
    for(int i=0; i<4; i++){
        printf("C%d: %d\n", i+1, node->key[i].C);
        if (f_type == 1) printf("Pr_%d: %d\n", i+1, node->key[i].Pr.rrn);
        else printf("Pr_%d: %ld\n", i+1, node->key[i].Pr.offset);
    }
    for(int i=0; i<5; i++){
        printf("P%d: %d\n", i+1, node->P[i]);
    }

    return;
}

// AUTOTAD_PRIVATE
/* Caso tenha ocorrido split na primeira chamada, houve split 
 * em um nó raiz; portanto, seu tipo é atualizando antes de 
 * ser reescrito no arquivo 
if (split_return == 1 && recursion_counter == 1){
    cur_node->tipoNo = '1';
}
*/
