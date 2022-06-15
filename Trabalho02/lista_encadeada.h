#ifndef LISTA_ENCADEADA_H
#define LISTA_ENCADEADA_H

#include "lista_encadeada.h"
#include "index.h"

/*
 * Comment section
*/
typedef struct index Index;

/*
 * Comment section
*/
typedef struct node Node;

/*
 * Comment section
*/
typedef struct list List;

/*
 * Comment section
*/
List* CreateList();

/*
 * Comment section
*/
int IsEmptyList(List *list);

/*
 * Comment section
*/
int AddLastElemList(List *list, Index e);

/*
 * Comment section
*/
int AddFirstElem(List *list, Index e);

/*
 * Comment section
*/
int AddMiddleElem(List *list, Index e, int index);

/*
 * Comment section
*/
int RemoveLastElemList(List *list, Index *e);

/*
 * Comment section
*/
int RemoveFirstElem(List *list, Index *e);

/*
 * Comment section
*/
int RemoveOddEven(List *list, Index *e);

/*
 * Comment section
*/
int InList(List *list, Index e);

/*
 * Comment section
*/
Node* SearchElemList(List *list, Index e, int *error, int *index);

/*
 * Comment section
*/
int SearchRemoveElem(List *list, Index *e, Index *dest_e);

/*
 * Comment section
*/
int IndexRemoveElem(List *list, Index *e, int index);

/*
 * Comment section
*/
int RemoveMiddleElem(Node *node, Index *e);

/*
 * Comment section
*/
int PrintList(List *list);

/*
 * Comment section
*/
int FreeList(List *list);

/*
 * Comment section
*/
Node *sort_merge(Node *node_left, Node *node_right);

/*
 * Comment section
*/
void split_mergesort(Node *src_node, Node **node_left, Node **node_right);

/*
 * Comment section
*/
void mergesort(Node **node_ref);

/*
 * Comment section
*/
List* sort_by_merge(List *list);

/*
 * Comment section
*/
Node* GetNextNode(Node *node);

/*
 * Comment section
*/
Node* GetFirstNode(List *list);

/*
 * Comment section
*/
Index GetElem(Node *node);


#endif