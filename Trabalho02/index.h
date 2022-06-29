#ifndef INDEX_H
#define INDEX_H

#include "./records_utils.h"

/*
 * Comment section
*/
typedef struct header Header;

/*
 * Comment section
*/
typedef struct idxHeader IdxHeader;

/*
 * Comment section
*/
typedef struct index Index;

/*
 * Comment section
*/
Index create_index(int f_type);

/*
 * Comment section
*/
int write_idx_header(FILE *file_idx_w);

/*
 * Comment section
*/
Index* load_all_idx_from_bin(FILE *file_bin_r, int f_type, int *n_indices, Header *header);

/*
 * Comment section
*/
int write_idx_in_bin_type1(FILE *file_idx_w, Index I);

/*
 * Comment section
*/
int write_idx_in_bin_type2(FILE *file_idx_w, Index I);

/*
 * Comment section
*/
int write_idx_file_from_bin(FILE *file_bin_r, Header *header, char *output_idx_filename, int f_type);

/*
 * Comment section
*/
int read_idx_type1(FILE *file_idx_r, Index *I);

/*
 * Comment section
*/
int read_idx_type2(FILE *file_idx_r, Index *I);

/*
 * Comment section
*/
void print_index(Index I, int f_type);

/*
 * Comment section
*/
void swap(Index *a, Index *b);

/*
 * Comment section
*/
void quick_sort(Index *I, int ini, int fim);

/*
 * Comment section
*/
Index* load_all_indices_from_idx(FILE *file_idx_r, int f_type, int *n_indices);

/*
 * Comment section
*/
int binary_search_idx(Index *I_list, int key, int ini, int fim) ;

/*
 * Comment section
*/
long int search_index_from_idx(Index *I_list, int n_indices, int key_id, int f_type);

/*
 * Comment section
*/
int add_new_index(Index **I_list, int *n_indices, int id, long int new_rrn_byteoffset, int f_type);

/*
 * Comment section
*/
int remove_index(Index **I_list, int *n_indices, int id);

/*
 * Comment section
*/
int update_index(Index **I_list, int *n_indices, int id, long int new_rrn_byteoffset, int f_type);

/*
 * Comment section
*/
int refresh_idx_file(char *f_idx, Index *I_list, int n_indices, int f_type);

/*
 * Comment section
*/
int get_rrn(Index *I_list, int *n_indices, int id);

/*
 * Comment section
*/
char get_idx_status(FILE *file_idx_r);


#endif