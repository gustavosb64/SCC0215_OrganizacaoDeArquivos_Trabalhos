#ifndef FUNCTIONS_H
#define FUNCTIONS_H


/*
 * Estrutura contendo os dados do cabeçalho dos arquivos de registro binários
*/
typedef struct header Header;

/*
 * Estrutura contendo os campos dos registros do tipo Vehicle (veículos)
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
char *readfield(FILE *stream) ;

/*
 * Comment section
*/
void binarioNaTela(char *nomeArquivoBinario) ; /* Você não precisa entender o código dessa função. */

/*
 * Função para leitura de dados de um csv; utiliza caractere ',' como separador
 *  Parâmetros:
 *      FILE* stream: arquivo de onde será lida a string
 *  Retorno:
 *      char*: string lida de stream
*/
char *read_data(FILE *stream) ;

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
 * Inicializa uma estrutura do tipo Vehicle com valores padrão.
 *  Parâmetros:
 *      int f_type: o tipo do arquivo a ser manipulado para inicialiar o veículo
 *                  com os valores certos
 *  Retorno:
 *      Estrutura Vehicle com os valores padrão
*/
Vehicle initialize_vehicle(int f_type);

/*
 * Inicializa registros do tipo 1 (tamanho fixo), preenchendo com caracteres '$' 
 * o espaço onde o registro será escrito no arquivo de saída.
 *  Parâmetros:
 *      FILE *file_bin_w: arquivo em que o registro será escrito 
 *  Retorno:
 *      0: nenhum erro
*/
int initialize_reg_type1(FILE *file_bin_w);

/*
 * Escreve o cabeçalho, tanto do tipo 1 quanto do tipo 2, no arquivo de saída.
 *  Parâmetros:
 *      FILE *file_bin_w: arquivo em que o cabeçalho será escrito 
 *      int f_type: tipo do registro a ser armazenado
 *  Retorno:
 *      0: nenhum erro
*/
int write_header(FILE *file_header_w, int f_type);

/*
 * Escreve um registro do tipo 1 em um arquivo binário 
 *  Parâmetros:
 *      FILE *file_bin_w: arquivo em que o registro será escrito 
 *      Vehicle *V: registro do tipo Vehicle contendo os dados a serem 
 *                  armazenados no arquivo
 *  Retorno:
 *      0: nenhum erro
*/
int write_reg_in_bin_type1(FILE *file_bin_w, Vehicle *V);

/*
 * Escreve um registro do tipo 2 em um arquivo binário 
 *  Parâmetros:
 *      FILE *file_bin_w: arquivo em que o registro será escrito 
 *      Vehicle *V: registro do tipo Vehicle contendo os dados a serem 
 *                  armazenados no arquivo
 *      int *size_last_reg: tamanho do último registro escrito
 *  Retorno:
 *      0: nenhum erro
*/
int write_reg_in_bin_type2(FILE *file_bin_w, Vehicle *V, int *size_last_reg);

/*
 * Lê um registro do tipo 1 com dado RRN de um arquivo binário
 *  Parâmetros:
 *      FILE *file_bin_r: arquivo do qual o registro será lido 
 *      Vehicle *V: registro do tipo Vehicle contendo os dados lidos do
 *                  arquivo fornecido
 *      int rrn: RRN do registro a ser lido
 *  Retorno:
 *      0: nenhum erro
*/
int read_reg_from_bin_type1(FILE *file_bin_r, Vehicle *V, int rrn);

/*
 * Lê um registro do tipo 2 com dado offset de um arquivo binário
 *  Parâmetros:
 *      FILE *file_bin_r: arquivo do qual o registro será lido 
 *      Vehicle *V: registro do tipo Vehicle contendo os dados lidos do
 *                  arquivo fornecido
 *      long int offset: offset do registro a ser lido
 *  Retorno:
 *      0: nenhum erro
*/
int read_reg_from_bin_type2(FILE *file_bin_r, Vehicle *V, long int *offset);

/*
 * Lê todos os registros de um arquivo binário
 *  Parâmetros:
 *      char *filename_in_bin: nome do arquivo binário de entrada
 *      int f_type: tipo do registro a ser lido
 *  Retorno:
 *      0: nenhum erro
 *      1: arquivo de entrada não encontrado
*/
int read_all_reg_from_bin(char *filename_in_bin, int f_type);

/*
 * Lê um registro de um arquivo csv
 *  Parâmetros:
 *      FILE *file_csv_r: arquivo csv do qual o registro será lido 
 *      Vehicle *V: registro de veículo onde os dados lidos serão armazenados
 *  Retorno:
 *      0: nenhum erro
 *      1: não há mais dados no arquivo de entrada
*/
int read_reg_from_csv(FILE *file_csv_r, Vehicle *V);

/*
 * Escreve um arquivo binário a partir de um arquivo csv
 *  Parâmetros:
 *      FILE *filename_in_csv: nome do arquivo de entrada csv 
 *      FILE *filename_out_bin: nome do arquivo de saída binário 
 *      int f_type: tipo do registro a ser escrito
 *  Retorno:
 *      0: nenhum erro
*/
int write_bin_from_csv(char *filename_in_csv, char *filename_out_bin, int f_type);

/*
 * Comment section
*/
int print_vehicle_full(Vehicle V, int f_type);

/*
 * Imprime os dados de um registro Vehicle
 *  Parâmetros:
 *      Vehicle V: estrutura cujos dados serão impressos
 *      int f_type: tipo do registro
 *  Retorno:
 *      0: nenhum erro
*/
int print_vehicle(Vehicle V, int f_type);

/*
 * Libera os dados dinamicamente alocados em uma estrutura Vehicle
 *  Parâmetros:
 *      Vehicle V: estrutura cujos dados serão liberados
 *  Retorno:
 *      0: nenhum erro
*/
int free_vehicle(Vehicle *V);

/*
 * Comment section
*/
int search_vehicle_rrn(char *filename_in_bin ,int rrn) ;

/*
 * Comment section
*/
char* remove_quotes_str(char* quoted_str) ;

/*
 * Comment section
*/
int customized_strcmp(char *v_str, char *str);

/*
 * Comment section
*/
int check_meets_condition(Vehicle V, char* field, char* value) ;

/*
 * Comment section
*/
int read_condition_reg_from_bin(char *filename_in_bin, int f_type, char** conditions, int n);


#endif