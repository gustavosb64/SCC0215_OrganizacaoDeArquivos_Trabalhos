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

struct b_Header{
    char status;    // consistência do arquivo
    int noRaiz;     // RRN do nó raiz 
    int proxRRN;    // próximo RRN disponível para conter um nó
    int nroNos;     // quantidade de nós presentes na árvore
};

/* Type1: 45 bytes
 * Type2: 56 bytes */
struct node{
    char tipoNo;    // tipo do nó; 0 = raiz, 1 = intermediário, 2 = folha
    int nroChaves;  // número de chaves presentes no nó

    // Referentes às chaves e RRN/offset no arquivo de dados

    //
    int C[3]; 
    union{
        int type1[3];
        int type2[3];
    }Pr;

    /*
    // Chave C1 
    int C1;
    union{
        int v1;
        long v2;
    }Pr_1;

    // Chave C2 
    int C2;
    union{
        int v1;
        long v2;
    }Pr_2;

    // Chave C3 
    int C3;
    union{
        int v1;
        long v2;
    }Pr_3;
    */

    // Referentes a subárvores no arquivo da Árvore-B
    int P[4]; 
    /*
    int P1; // key < C1
    int P2; // C1 < key < C2
    int P3; // C2 < key < C3
    int P4; // C3 < key
    */
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

/* 
 * Inicializa uma variável do tipo Node com valores padrão
*/
Node* initialize_node(int f_type){

    Node *node = (Node*) malloc(sizeof(Node));

    node->tipoNo = '2';     // iniciando o nó como uma folha
    node->nroChaves = 0;

    for(int i=0; i<3; i++){

        node->C[i] = -1;
        if (f_type == 1) node->Pr.type1[i] = -1;
        else node->Pr.type2[i] = -1;

        node->P[i] = -1;
    }
    node->P[3] = -1;

    /*
    node->C1 = -1;
    if (f_type == 1) node->Pr_1.v1 = -1;
    else node->Pr_1.v2 = -1;

    node->C2 = -1;
    if (f_type == 2) node->Pr_2.v1 = -1;
    else node->Pr_2.v2 = -1;

    node->C3 = -1;
    if (f_type == 1) node->Pr_3.v1 = -1;
    else node->Pr_3.v2 = -1;
    */

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

/* AUTOTAD_PRIVATE
int initialize_node(FILE *file_btree_w, int f_type){

    // Inicializa espaço em arquivo com lixo
    char aux_char = '$';
    int size; 
    if (f_type == 1) size = NODE_SIZE_TYPE1;
    else size = NODE_SIZE_TYPE2;

    fwrite(&aux_char, sizeof(char), size, file_btree_w); 

    return 0;
}
*/

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
    Node *node = (Node*) malloc(sizeof(Node));

    fread(&(node->tipoNo), sizeof(char), 1, file_btree_r);
    fread(&(node->nroChaves), sizeof(int), 1, file_btree_r);

    // Lê chaves de referência
    for(int i=0; i < 3; i++){
        fread(&(node->C[i]), sizeof(int), 1, file_btree_r);
        if (f_type == 1) fread(&(node->Pr.type1[i]), sizeof(int), 1, file_btree_r);
        else fread(&(node->Pr.type2[i]), sizeof(long int), 1, file_btree_r);
    }
    
    // Lê referência para outros nós da árvore
    for(int i=0; i < 4; i++){
        fread(&(node->P[i]), sizeof(int), 1, file_btree_r);
    }

    return node;
}

long int search_in_page_b_tree(FILE *file_btree_r, Node *cur_node, int src_id, int f_type){

    /*
    print_node(cur_node, 1);
    printf("-------------\n");
    */

    // Armazena em Pr[] os valores correspondentes do nó de acordo com o tipo do arquivo
    long int Pr[4];
    if (f_type == 1){
        for (int i=0; i<4; i++)
            Pr[i] = cur_node->Pr.type1[i];
    }
    else{
        for (int i=0; i<4; i++)
            Pr[i] = cur_node->Pr.type2[i];
    }

    // Itera por cada uma das chaves comparando com o ID buscado
    for (int i=0; i < cur_node->nroChaves; i++){
        //printf("i: %d, src_id: %d, cur_node[C]: %d\n",i, src_id, cur_node->C[i]);

        // Caso a chave atual seja igual ao ID buscado, retorna sua referência
        if (cur_node->C[i] == src_id){
            free(cur_node);
            return Pr[i];
        }

        if (cur_node->C[i] > src_id){
            
            /*
            printf("Maior:\n");
            printf("Pr[%d]: %ld\n",i-1, Pr[i-1]);
            */
            
            Node *new_node = read_node_from_b_tree(file_btree_r, cur_node->P[i], f_type);
            free(cur_node);
            if (new_node == NULL){
                free(new_node);
                return -1;
            }

            /*
            print_node(new_node, f_type);
            printf("-------------\n");
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

    // Declarando valor de retorno (i.e., índice no arquivo de dados)
    long int ref;
    if (f_type == 1) ref = -1;
    else ref = -1;
    
    /*
    printf("noRaiz: %d\n", b_header->noRaiz);
    printf("src_id: %d\n", src_id);
    printf("\n");
    */

    // Busca referência na Árvore-B
    Node *node = read_node_from_b_tree(file_btree_r, b_header->noRaiz, f_type);
    ref = search_in_page_b_tree(file_btree_r, node, src_id, f_type);

    return ref;
}

/* 
 * Imprime os valores de uma estrutura do tipo Node
*/
void print_node(Node *node, int f_type){
    
    printf("tipoNo: %c\n", node->tipoNo);
    printf("nroChaves: %d\n", node->nroChaves);
    for(int i=0; i<3; i++){
        printf("C%d: %d\n", i+1, node->C[i]);
        if (f_type == 1) printf("Pr_%d: %d\n", i+1, node->Pr.type1[i]);
        else printf("Pr_%d: %d\n", i+1, node->Pr.type2[i]);
    }
    for(int i=0; i<4; i++){
        printf("P%d: %d\n", i+1, node->P[i]);
    }

    return;
}
