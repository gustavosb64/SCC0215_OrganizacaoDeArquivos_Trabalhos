#ifndef LISTA_ENCADEADA_H
#define LISTA_ENCADEADA_H

typedef struct index elem;

typedef struct node Node;
typedef struct list List;

List* CreateList();
int IsEmpty(List *list);
int AddLastElemList(List *list, elem e);
int AddFirstElem(List *list, elem e);
int AddMiddleElem(List *list, elem e, int index);
int RemoveLastElemList(List *list, elem *e);
int RemoveFirstElem(List *list, elem *e);
int RemoveOddEven(List *list, elem *e);
int InList(List *list, elem e);
Node* SearchElem(List *list, elem e, int *error, int *index);
int SearchRemoveElem(List *list, elem *e, elem *dest_elem);
int IndexRemoveElem(List *list, elem *e, int index);
int RemoveMiddleElem(Node *node, elem *e);
int PrintList(List *list);
int FreeList(List *list);
List* sort_by_merge(List *list);

#endif
