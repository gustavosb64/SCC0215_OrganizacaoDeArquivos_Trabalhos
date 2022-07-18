#ifndef B_TREE_H
#define B_TREE_H


typedef struct b_Header B_Header;

typedef struct key Key;

typedef struct node Node;

B_Header* initialize_btree_header();

int update_btree_header(FILE *file_btree_rw, B_Header *b_header);

Node* initialize_node(int f_type);

int write_b_tree_header(FILE *file_btree_w, B_Header *b_header);

B_Header* read_header_from_btree(FILE *file_btree_r);

Node* read_node_from_b_tree(FILE *file_btree_r, int rrn_b_tree, int f_type);

long int search_in_page_b_tree(FILE *file_btree_r, Node *cur_node, int src_id, int f_type);

long int search_index_in_b_tree(FILE *file_bin_r, FILE *file_btree_r, int src_id, B_Header *b_header, Header *f_header, int f_type);

int isLeafNode(Node *node);

int write_node_in_btree_file(FILE *file_btree_rw, Node *node, int rrn, int f_type);

void test_write_node();

int split_node(FILE *file_btree_rw, Node *cur_node, B_Header *b_header, int f_type, Key *promo_key, int *promo_r_child);
 
int insert_new_id_in_node(Node *node, Key new_key);
 
void test_insertion_sort();

int insert_subtree_rrn_in_node(Node *node, int subtree_rrn, int index);

int insert_btree(FILE *file_btree_rw, B_Header *b_header, Key new_key, int cur_rrn_btree, int f_type, Key *promo_key, int *promo_r_child);

int add_new_node_btree(FILE *file_btree_rw, B_Header *b_header, int id, long int id_ref, int f_type);

void print_node(Node *node, int f_type);


#endif