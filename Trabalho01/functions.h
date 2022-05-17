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
int write_header_type1(char *filename);

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
typedef struct vehicle Vehicle;

/*
 * Comment section
*/
Vehicle initialize_vehicle();

/*
 * Comment section
*/
int read_csv_type1(char *filename);

/*
 * Comment section
*/
int print_vehicle(Vehicle V);


#endif