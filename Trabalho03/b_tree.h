#ifndef B_TREE_H
#define B_TREE_H


typedef struct b_Header B_Header;

typedef struct node Node;

B_Header* initialize_btree_header();

Node* initialize_node(int f_type);

int write_b_tree_header(FILE *file_btree_w, B_Header *b_header);

B_Header* read_header_from_btree(FILE *file_btree_r);

Node* read_node_from_b_tree(FILE *file_btree_r, int rrn_b_tree, int f_type);

long int search_in_page_b_tree(FILE *file_btree_r, Node *cur_node, int src_id, int f_type);

long int search_index_in_b_tree(FILE *file_bin_r, FILE *file_btree_r, int src_id, B_Header *b_header, Header *f_header, int f_type);

void print_node(Node *node, int f_type);


#endif