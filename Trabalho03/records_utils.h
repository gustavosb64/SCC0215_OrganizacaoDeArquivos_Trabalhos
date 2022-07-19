#ifndef RECORDS_UTILS_H
#define RECORDS_UTILS_H

#include "./index.h"
#include "./b_tree.h"

/*
 * Estrutura contendo os dados do cabeçalho dos arquivos de registro binários
*/
typedef struct header Header;

/*
 * Estrutura contendo os campos dos registros do tipo Vehicle (veículos)
*/
typedef struct vehicle Vehicle;

/*
 * Função de leitura de linha.
 *  Parâmetros:
 *      FILE *stream: stream de onde será lida a string
 *      char delimiters[]: delimitadores, além de '\n', '\r' e EOF, que interrompe leitura
*/
char* readline(FILE *stream, char delimiters[]);

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
 * Escreve o cabeçalho, tanto do tipo 1 quanto do tipo 2, no arquivo de saída.
 *  Parâmetros:
 *      FILE *file_bin_w: arquivo em que o cabeçalho será escrito 
 *      int f_type: tipo do registro a ser armazenado
 *  Retorno:
 *      0: nenhum erro
*/
int write_header(FILE *file_header_w, int f_type);

/*
 * Comment section
*/
Header* initialize_header(int f_type);

/*
 * Comment section
*/
Header* read_header_from_bin(FILE *file_bin_r, int f_type);

/*
 * Comment section
*/
int update_header(FILE *file_bin_rw, Header *H, int f_type);

/*
 * Lê todos os registros de um arquivo binário
 *  Parâmetros:
 *      char *filename_in_bin: nome do arquivo binário de entrada
 *      int f_type: tipo do registro a ser lido
 *  Retorno:
 *      0: nenhum erro
 *      1: falha na abertura do arquivo, erro na leitura do campo, fim do arquivo atingido 
*/
int read_all_reg_from_bin(char *filename_in_bin, int f_type);

/*
 * Lê um registro de um arquivo csv
 *  Parâmetros:
 *      FILE *file_csv_r: arquivo csv do qual o registro será lido 
 *      Vehicle *V: registro de veículo onde os dados lidos serão armazenados
 *  Retorno:
 *      0: nenhum erro
 *      1: não há mais campos no csv a serem lidos (leitura do campo id retornou uma string vazia)
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
 *      1: falha na abertura do arquivo de leitura csv
*/
int write_bin_from_csv(char *filename_in_csv, char *filename_out_bin, int f_type);

/*
 * Remove aspas da string passada por parâmetro.
 *  Parâmetros:
 *      char *quoted_str: string a ter aspas removidas
 *  Retorno:
 *      char *: string tratada, com aspas removidas
*/
char* remove_quotes_str(char* quoted_str) ;

/*
 * Função customizada para comparação de string. Adiciona caractere '\0' ao final de v_str para utilizar strcmp()
 * Utilizada pela função check_meets_condition.
 *  Parâmetros:
 *      char *v_str: string armazenada em uma estrutura do tipo Vehicle (sem '\0' ao final)
 *      char *str: string usada para comparação
 *  Retorno:
 *      strcmp(v_str, str)
*/
int customized_strcmp(char *v_str, char *str);

/*
 * Checa se o campo lido cumpre com os requisitos de filtragem.
 *  Parâmetros:
 *      Vehicle V: estrutura onde os dados do registro lido estão armazenados
 *      char *field: nome do campo a ser usado na comparação 
 *      char *value: valor do campo a ser usado na comparação
 *  Retorno:
 *      0: não cumpre com o requisito (comparação retorna falso)
 *      1: cumpre com o requisito (comparação retorna verdadeiro)
*/
int check_meets_condition(Vehicle V, char* field, char* value, int quoted) ;

/*
 * Lê registros de um arquivo binário filtrando por condições passadas por parâmetro.
 *  Parâmetros:
 *      char *filename_in_bin: nome do arquivo binário de leitura
 *      int f_type: tipo do arquivo
 *      char **conditions: condições de filtragem
 *      int n: número de condições
 *  Retorno:
 *      0: nenhum erro
 *      1: falha na abertura do arquivo de leitura
*/
int read_condition_reg_from_bin(char *filename_in_bin, int f_type, char** conditions, int n);

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
 * Imprime todos os dados de um registro Vehicle
 *  Parâmetros:
 *      Vehicle V: estrutura cujos dados serão impressos
 *      int f_type: tipo do registro
 *  Retorno:
 *      0: nenhum erro
*/
int print_vehicle_full(Vehicle V, int f_type);

/*
 * Imprime os dados requisitados pela aplicação de um registro Vehicle
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
 * Adiciona novo registro no arquivo de dados
*/
int add_new_reg(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, Header *header, char *id, char *ano, char *qtt, char *sigla, char *cidade, char *marca, char *modelo);

/*
 * Altera o próximo RRN ou offset no arquivo
*/
int update_prox(FILE *file_bin_rw, int f_type, long int new_value);

/*
 * Seta o status de um arquivo com o que está em status. Deve ser '1' ou '0';
*/
int set_status_file(FILE *file_rw, char status);

/*
 * Lê status do arquivo em file_bin_r
*/
char get_status(FILE *file_bin_r);

/*
 * Lê status presente no ponteiro header
*/
char get_status_from_header(Header *header);

/*
 * Remove registro de um arquivo binário fornecidos em fields
*/
int delete_bin(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, int n, char** fields, char** values, Header *header) ;

/*
 * Configura strings para utilização em update_vehicle
*/
void setup_vehicle_strings(Vehicle *V) ;

/*
 * Atualiza veículo em estrutura V, alterando seus campos e os atualizando
*/
void update_vehicle(Vehicle *V, int n, char** fields, char** values, Index **I_list, int n_indices, int f_type) ;

/*
 * Atualiza veículos no arquivo de dados com os dados presentes em values e campos de fieelds
*/
int update_bin(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, int x, char** search_fields, char** search_values, int y, char** update_fields, char** update_values, Header *header) ;

int search_reg_in_btree(FILE *file_bin_r, FILE *file_btree_r, int src_id, Header *f_header, B_Header *b_header, int f_type);

int add_new_reg_using_btree(FILE *file_bin_rw, FILE *file_btree_rw, int f_type, Header *f_header, B_Header *b_header, char *id, char *ano, char *qtt, char *sigla, char *cidade, char *marca, char *modelo);


#endif