#ifndef INDEX_H
#define INDEX_H


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
Index* load_all_idx_from_bin(FILE *file_bin_r, int f_type, int *n_indices);

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
int write_idx_file_from_bin(char *input_filename, char *output_filename, int f_type);

/*
 * Comment section
*/
int read_idx_type1(FILE *file_idx_r, Index *I, int idx_rrn);

/*
 * Comment section
*/
int read_all_indices_from_idx(char *input_filename, int f_type);

/*
 * Comment section
*/
int search_index_from_idx(char *input_filename, int src_id, int f_type);

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
int set_status_idx(FILE *file_idx_rw, char status);

/*
 * Comment section
*/
Index* load_all_indices_from_idx(FILE *file_idx_r, int f_type);

/*
 * Comment section
*/
int refresh_idx(char *bin_filename, char *idx_filename, int f_type);


#endif