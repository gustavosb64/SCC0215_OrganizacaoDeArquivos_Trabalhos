#include <stdio.h>
#include <stdlib.h>
#include "lista_encadeada.h"
#include "index.h"

struct index{
    int id;
    union{
        int rrn;
        long int byteoffset;
    }idx;
};

struct node{
    Index val;
    struct node *next;
    struct node *prev;
};

struct list{
    Node *first;
    Node *last;
    int n_elem;
};

List* CreateList(){
    List *list = (List *) malloc(sizeof(List)); 
    if (list == NULL){
        printf("CreateList FAILED: Memory is full.\n");
        return NULL;
    }
    
    list->first = NULL; 
    list->last = list->first; 

    list->n_elem = 0; 

    return list;
}

int IsEmptyList(List *list){
    if (list->n_elem == 0) return 1;
    else return 0;
}

int AddLastElemList(List *list, Index e){
    Node *aux_node = (Node *) malloc(sizeof(Node));
    if (aux_node == NULL) return 1;

    if (IsEmptyList(list)){
        list->first = aux_node;
        list->first->prev = NULL;
    }
    else {
        list->last->next = aux_node; 
        aux_node->prev = list->last;
    }

    list->last = aux_node;

    list->last->val = e;
    list->last->next = NULL;

    list->n_elem++;

    return 0;
}

int AddFirstElem(List *list, Index e){
    Node *aux_node = (Node *) malloc(sizeof(Node));
    if (aux_node == NULL) return 1;

    if (IsEmptyList(list)){
        list->first = aux_node;
        list->first->prev = NULL;
    }
    else {
        list->first->prev = aux_node; 
        aux_node->next = list->first;
    }

    list->first = aux_node;

    list->first->val = e;
    list->first->prev = NULL;

    list->n_elem++;
    return 0;
}

int AddMiddleElem(List *list, Index e, int index){

    if (index == 0){
        if (AddFirstElem(list, e)) return 1;
        return 0;
    }

    if (index == (list->n_elem - 1)){
        if (AddLastElemList(list, e)) return 1;
        return 0;
    }

    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL) return 1;

    Node *aux = list->first;
    int i=1;
    while (i < index){
        aux = aux->next;
        i++;
    }

    node->next = aux->next;
    aux->next = node;
    node->prev = aux;

    node->val = e;

    return 0;
}

int RemoveLastElemList(List *list, Index *e){

    if(IsEmptyList(list)) return 1;

    *e = list->last->val;
    
    Node *aux_node = list->last->prev;

    if (aux_node != NULL)
        aux_node->next = NULL; 

    free(list->last);
    list->last = aux_node;

    list->n_elem--;

    return 0;
}

int RemoveFirstElem(List *list, Index *e){

    if(IsEmptyList(list)) return 1;

    *e = list->first->val;
    
    Node *aux_node = list->first->next;

    if (aux_node != NULL)
        aux_node->prev = NULL; 

    free(list->first);
    list->first = aux_node;

    list->n_elem--;

    return 0;
}

int RemoveOddEven(List *list, Index *e){
    
    if(IsEmptyList(list)) return 1;

    if((list->n_elem)%2) RemoveFirstElem(list, e);
    else RemoveLastElemList(list, e);

    return 0;
}

int InList(List *list, Index e){

    if(IsEmptyList(list)) return -1;

    AddLastElemList(list, e);

    Node *node = list->first;
    while (node->val.id != e.id)
        node = node->next;

    int check = 1;
    if (node->next == NULL) check = 0;
    
    RemoveLastElemList(list, &e);

    return check;
}

Node* SearchElemList(List *list, Index e, int *error, int *index){

    if(IsEmptyList(list)){
        *error = 1;
        return NULL;
    }

    Node *dest_node;
    dest_node = list->first;
    *index = 0;

    while (dest_node != NULL){
        if (dest_node->val.id == e.id) return dest_node;
        dest_node = dest_node->next;
        *index = *index+1;
    }
    
    *error = 2;
    return NULL;
}

int SearchRemoveElem(List *list, Index *e, Index *dest_e){

    if(IsEmptyList(list)) return 1;

    Node *dest_node;
    int error = 0;
    int index = 0;

    dest_node = SearchElemList(list, *e, &error, &index);
    switch (error){
        case 1: 
            return 1;
        case 2:
            printf("Element not found!\n");
            free(dest_node);
            return 0;
    }

    if (index == 0){
        if (RemoveFirstElem(list, dest_e)) return 1;
    }
    else if (index == (list->n_elem-1)){
        if (RemoveLastElemList(list, dest_e)) return 1;
    }
    else{
        RemoveMiddleElem(dest_node, dest_e);
        list->n_elem--;
    }

    return 0;
}

int IndexRemoveElem(List *list, Index *e, int index){

    if(IsEmptyList(list)) return 1;
    if (index > list->n_elem) return 2;
    if (index < 0) return 3;
    
    if (index == 0){
        if (RemoveFirstElem(list, e)) return 1;
        return 0;
    }

    if (index == (list->n_elem - 1)){
        if (RemoveLastElemList(list, e)) return 1;
        return 0;
    }

    Node *aux = list->first;
    int i=1;
    while (i <= index){
        aux = aux->next;
        i++;
    }
   
    RemoveMiddleElem(aux, e);
    list->n_elem--;

    return 0;
}

int RemoveMiddleElem(Node *node, Index *e){

    if(node == NULL) return 1;
    if (node->prev == NULL) return 2;
    if (node->next == NULL) return 3;

    node->prev->next = node->next;    
    node->next->prev = node->prev;
    
    node->next = NULL;
    node->prev = NULL;
    *e = node->val;

    free(node); 

    return 0;
}

int PrintList(List *list){

    if(IsEmptyList(list)) return 1;
    
    Node *aux_node = list->first;
    
    while(aux_node!=NULL){
        printf("%d\n",aux_node->val.id);
        aux_node = aux_node->next;
    }

    return 0;
}

int FreeList(List *list){
    
    if(IsEmptyList(list)){
        free(list->first); 
        free(list);
        return 1;
    }

    Node *aux_node = list->first;
    Node *aux_node2;
    while(aux_node!=NULL){
        aux_node2 = aux_node;
        aux_node = aux_node->next;
        free(aux_node2);
    }
    free(aux_node);
    free(list);

    return 0;
}

/* MERGESORT FUNCTIONS */
Node *sort_merge(Node *node_left, Node *node_right){

    Node *node_res = NULL;
    
    if (node_left == NULL) return node_right;
    if (node_right == NULL) return node_left;

    if (node_left->val.id <= node_right->val.id){
        node_res = node_left;
        node_res->next = sort_merge(node_left->next, node_right);
    }
    else{
        node_res = node_right;
        node_res->next = sort_merge(node_left, node_right->next);
    }

    return node_res;
}

void split_mergesort(Node *src_node, Node **node_left, Node **node_right){

    Node *slow_node = src_node;
    Node *fast_node = slow_node->next;

    while (fast_node){
        fast_node = fast_node->next;
        if (fast_node){
            slow_node = slow_node->next;
            fast_node = fast_node->next;
        }
    }

    (*node_left) = src_node;
    (*node_right) = slow_node->next;
    slow_node->next = NULL;

    return;
}

void mergesort(Node **node_ref){

    Node *head = *node_ref;
    Node *node_left;
    Node *node_right;

    if (head == NULL || head->next == NULL)
        return;

    split_mergesort(head, &node_left, &node_right);

    mergesort(&node_left);
    mergesort(&node_right);

    (*node_ref) = sort_merge(node_left, node_right);
}

List* sort_by_merge(List *list){

    mergesort(&(list->first));

    return list;
}
