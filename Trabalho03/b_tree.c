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

    int C1;
    union{
        int v1;
        long v2;
    }Pr_1;

    int C2;
    union{
        int v1;
        long v2;
    }Pr_2;

    int C3;
    union{
        int v1;
        long v2;
    }Pr_3;

    int P1;
    int P2;
    int P3;
    int P4;
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

    node->C1 = -1;
    if (f_type == 1) node->Pr_1.v1 = -1;
    else node->Pr_1.v2 = -1;

    node->C2 = -1;
    if (f_type == 2) node->Pr_2.v1 = -1;
    else node->Pr_2.v2 = -1;

    node->C3 = -1;
    if (f_type == 1) node->Pr_3.v1 = -1;
    else node->Pr_3.v2 = -1;

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
