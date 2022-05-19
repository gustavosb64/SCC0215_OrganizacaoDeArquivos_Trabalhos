#ifndef FUNCTIONS_H
#define FUNCTIONS_H


/*
 * Comment section
*/
typedef struct header Header;

/*
 * Comment section
*/
typedef struct vehicle Vehicle;

/*
 * Função para leitura de string de um arquivo
 *  Parâmetros:
 *      FILE* stream: arquivo de onde será lida a string
 *  Retorno:
 *      char*: string lida de stream
*/
char *readline(FILE *stream) ;

/*
 * Comment section
*/
char *read_data(FILE *stream) ;

/*
 * Comment section
*/
int write_header_type1(FILE *file_header_w);

/*
 * Função para imprimir strings dos registradores na saída padrão
 *  Parâmetros:
 *      char string[]: string a ser impressa
 *      int len: comprimento da string
 *  Retorno:
 *      0: nenhum erro
 *      1: comprimento fornecido inválido (menor ou igual a 0)
*/
int print_string(char string[], int len);

/*
 * Comment section
*/
int read_header_type1(char *filename);

/*
 * Comment section
*/
Vehicle initialize_vehicle(int type);

/*
 * Comment section
*/
int read_reg_from_bin_type1(FILE *file_bin_r, Vehicle *V, int rrn);

/*
 * Comment section
*/
int read_bin_all_reg(char *filename);

/*
 * Comment section
*/
int initialize_reg_type1(FILE *file_bin_w);

/*
 * Comment section
*/
int write_file_type1(FILE *file_bin_w, Vehicle *V);

/*
 * Comment section
*/
int read_reg_from_csv_type1(FILE *file_csv_r, Vehicle *V);

/*
 * Comment section
*/
int write_bin_from_csv_type1(char *filename);

/*
 * Comment section
*/
int print_vehicle(Vehicle V);

/*
 * Comment section
*/
int free_vehicle(Vehicle *V);


#endif