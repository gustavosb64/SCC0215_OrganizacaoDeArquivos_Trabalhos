#ifndef B_TREE_H
#define B_TREE_H


typedef struct b_Header B_Header;

typedef struct node Node;

B_Header* initialize_btree_header();

Node* initialize_node(int f_type);

int write_b_tree_header(FILE *file_btree_w, B_Header *b_header);


#endif