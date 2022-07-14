#ifndef INDEX_H
#define INDEX_H


typedef struct header Header;

typedef struct idxHeader IdxHeader;

typedef struct index Index;

Index create_index(int f_type);

int write_idx_header(FILE *file_idx_w);

Index* load_all_idx_from_bin(FILE *file_bin_r, int f_type, int *n_indices, Header *header);

int write_idx_in_bin_type1(FILE *file_idx_w, Index I);

int write_idx_in_bin_type2(FILE *file_idx_w, Index I);

int write_idx_file_from_bin(FILE *file_bin_r, Header *header, char *output_idx_filename, int f_type);

int read_idx_type1(FILE *file_idx_r, Index *I);

int read_idx_type2(FILE *file_idx_r, Index *I);

void print_index(Index I, int f_type);

void swap(Index *a, Index *b);

void quick_sort(Index *I, int ini, int fim);

Index* load_all_indices_from_idx(FILE *file_idx_r, int f_type, int *n_indices);

int binary_search_idx(Index *I_list, int key, int ini, int fim) ;

long int search_index_from_idx(Index *I_list, int n_indices, int key_id, int f_type);

int add_new_index(Index **I_list, int *n_indices, int id, long int new_rrn_byteoffset, int f_type);

int remove_index(Index **I_list, int *n_indices, int id);

int update_index(Index **I_list, int *n_indices, int id, long int new_rrn_byteoffset, int f_type);

int refresh_idx_file(char *f_idx, Index *I_list, int n_indices, int f_type);

int get_rrn(Index *I_list, int *n_indices, int id);

char get_idx_status(FILE *file_idx_r);


#endif