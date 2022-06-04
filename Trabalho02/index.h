#ifndef INDEX_H
#define INDEX_H


/*
 * Comment section
*/
typedef struct idxHeader Idxheader;

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
int read_id_from_reg_type1(FILE *file_bin_r, Index *I, int rrn);

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
int read_all_idices_from_idx(char *input_filename, int f_type);


#endif