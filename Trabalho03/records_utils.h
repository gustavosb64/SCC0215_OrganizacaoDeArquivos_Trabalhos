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
 *  @param:
 *      FILE *stream: stream de onde será lida a string
 *      char delimiters[]: delimitadores, além de '\n', '\r' e EOF, que interrompe leitura
 *  @return:
 *      char* : string lida de *stream
*/
char* readline(FILE *stream, char delimiters[]);

/*
 * Inicializa uma estrutura do tipo Vehicle com valores padrão.
 *  @param:
 *      int f_type: o tipo do arquivo a ser manipulado para inicialiar o veículo
 *                  com os valores certos
 *  @return:
 *      Estrutura Vehicle com os valores padrão
*/
Vehicle initialize_vehicle(int f_type);

/*
 * Escreve o cabeçalho, tanto do tipo 1 quanto do tipo 2, no arquivo de saída.
 *  @param:
 *      FILE *file_bin_w: arquivo em que o cabeçalho será escrito 
 *      int f_type: tipo do registro a ser armazenado
 *  @return:
 *      0: nenhum erro
*/
int write_header(FILE *file_header_w, int f_type);

/*
 * Inicializa uma variável do tipo Header* com valores padrão e o retorna
 *  @param:
 *      int f_type: tipo do registro de dados
 *  @return:
 *      Header*: novo cabeçalho
*/
Header* initialize_header(int f_type);

/*
 * Lê cabeçalho do arquivo binário de dados e o retorna
 *  @param:
 *      FILE* file_bin_r: arquivo binário de dados
 *      int f_type: tipo do registro de dados
 *  @return:
 *      Header*: novo cabeçalho com os dados lidos
*/
Header* read_header_from_bin(FILE *file_bin_r, int f_type);

/*
 * Atualiza cabeçalho em arquivo binário de dados
 *  @param:
 *      FILE* file_bin_r: arquivo binário de dados
 *      Header* header: cabeçalho com os dados a serem escritos
 *      int f_type: tipo do registro de dados
 *  @return:
 *      0: nenhum erro ocorreu
*/
int update_header(FILE *file_bin_rw, Header *H, int f_type);

/*
 * Lê todos os registros de um arquivo binário
 *  @param:
 *      char *filename_in_bin: nome do arquivo binário de entrada
 *      int f_type: tipo do registro a ser lido
 *  @return:
 *      0: nenhum erro
 *      1: falha na abertura do arquivo, erro na leitura do campo, fim do arquivo atingido 
*/
int read_all_reg_from_bin(char *filename_in_bin, int f_type);

/*
 * Lê um registro de um arquivo csv
 *  @param:
 *      FILE *file_csv_r: arquivo csv do qual o registro será lido 
 *      Vehicle *V: registro de veículo onde os dados lidos serão armazenados
 *  @return:
 *      0: nenhum erro
 *      1: não há mais campos no csv a serem lidos (leitura do campo id retornou uma string vazia)
*/
int read_reg_from_csv(FILE *file_csv_r, Vehicle *V);

/*
 * Escreve um arquivo binário a partir de um arquivo csv
 *  @param:
 *      FILE *filename_in_csv: nome do arquivo de entrada csv 
 *      FILE *filename_out_bin: nome do arquivo de saída binário 
 *      int f_type: tipo do registro a ser escrito
 *  @return:
 *      0: nenhum erro
 *      1: falha na abertura do arquivo de leitura csv
*/
int write_bin_from_csv(char *filename_in_csv, char *filename_out_bin, int f_type);

/*
 * Remove aspas da string passada por parâmetro.
 *  @param:
 *      char *quoted_str: string a ter aspas removidas
 *  @return:
 *      char *: string tratada, com aspas removidas
*/
char* remove_quotes_str(char* quoted_str) ;

/*
 * Função customizada para comparação de string. Adiciona caractere '\0' ao final de v_str para utilizar strcmp()
 * Utilizada pela função check_meets_condition.
 *  @param:
 *      char *v_str: string armazenada em uma estrutura do tipo Vehicle (sem '\0' ao final)
 *      char *str: string usada para comparação
 *  @return:
 *      strcmp(v_str, str)
*/
int customized_strcmp(char *v_str, char *str);

/*
 * Checa se o campo lido cumpre com os requisitos de filtragem.
 *  @param:
 *      Vehicle V: estrutura onde os dados do registro lido estão armazenados
 *      char *field: nome do campo a ser usado na comparação 
 *      char *value: valor do campo a ser usado na comparação
 *  @return:
 *      0: não cumpre com o requisito (comparação retorna falso)
 *      1: cumpre com o requisito (comparação retorna verdadeiro)
*/
int check_meets_condition(Vehicle V, char* field, char* value, int quoted) ;

/*
 * Lê registros de um arquivo binário filtrando por condições passadas por parâmetro.
 *  @param:
 *      char *filename_in_bin: nome do arquivo binário de leitura
 *      int f_type: tipo do arquivo
 *      char **conditions: condições de filtragem
 *      int n: número de condições
 *  @return:
 *      0: nenhum erro
 *      1: falha na abertura do arquivo de leitura
*/
int read_condition_reg_from_bin(char *filename_in_bin, int f_type, char** conditions, int n);

/*
 * Função para imprimir strings dos registradores na saída padrão
 *  @param:
 *      char string[]: string a ser impressa
 *      int len: comprimento da string
 *  @return:
 *      0: nenhum erro
 *      1: comprimento fornecido inválido (menor ou igual a 0)
*/
int print_string(char string[], int len);

/*
 * Imprime todos os dados de um registro Vehicle
 *  @param:
 *      Vehicle V: estrutura cujos dados serão impressos
 *      int f_type: tipo do registro
 *  @return:
 *      0: nenhum erro
*/
int print_vehicle_full(Vehicle V, int f_type);

/*
 * Imprime os dados requisitados pela aplicação de um registro Vehicle
 *  @param:
 *      Vehicle V: estrutura cujos dados serão impressos
 *      int f_type: tipo do registro
 *  @return:
 *      0: nenhum erro
*/
int print_vehicle(Vehicle V, int f_type);

/*
 * Libera os dados dinamicamente alocados em uma estrutura Vehicle
 *  @param:
 *      Vehicle V: estrutura cujos dados serão liberados
 *  @return:
 *      0: nenhum erro
*/
int free_vehicle(Vehicle *V);

/*
 * Adiciona novo registro no arquivo de dados
 *  @param:
 *      FILE* file_bin_rw: arquivo binário de dados
 *      int f_type: tipo do arquivo de dados
 *      Index** I_list: lista de índices carregada na memória
 *      int* n_indices: número de índices na lista
 *      Header* header: cabeçalho do arquivo de dados
 *      char* id: id do novo registro
 *      char* qtt: quantidade do novo registro
 *      char* sigla: sigla do novo registro
 *      char* cidade: cidade do novo registro
 *      char* marca: marca do novo registro
 *      char* modelo: modelo do novo registro
 *  @return:
 *      0: nenhum erro ocorreu
*/
int add_new_reg(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, Header *header, char *id, char *ano, char *qtt, char *sigla, char *cidade, char *marca, char *modelo);

/*
 * Altera o próximo RRN ou offset no arquivo
 *  @param:
 *      FILE* file_rw: arquivo binário cujo status será alterado
 *      int f_type: tipo do arquivo de dados
 *      long int new_value: nova referência de próximo RRN ou byte offset a ser escrita
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: f_type inválido
*/
int update_prox(FILE *file_bin_rw, int f_type, long int new_value);

/*
 * Seta o status de um arquivo com o que está em status. Deve ser '1' ou '0';
 *  @param:
 *      FILE* file_rw: arquivo binário cujo status será alterado
 *      char status: status que será escrito no arquivo binário
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: status inválido
*/
int set_status_file(FILE *file_rw, char status);

/*
 * Lê status do arquivo em file_bin_r
 *  @param:
 *      FILE* file_bin_r: arquivo binário de dados
 *  @return:
 *      int [status do arquivo] 
*/
char get_status(FILE *file_bin_r);

/*
 * Lê status presente no ponteiro header
 *  @param:
 *      Header* header: cabeçalho de um arquivo de dados
 *  @return:
 *      int header->status
*/
char get_status_from_header(Header *header);

/*
 * Remove registro de um arquivo binário fornecidos em fields
 *  @param:
 *      FILE* file_bin_rw: arquivo binário de dados
 *      int f_type: tipo do arquivo de dados
 *      Index** I_list: lista de índices carregada na memória
 *      int* n_indices: número de índices na lista
 *      int n: número de elementos a serem removidos
 *      int** fields: campos utilizados na busca para remoção
 *      char** values: valores dos campos utilizados na busca para remoção
 *      Header* header: cabeçalho do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: registro buscado não se encontra no arquivo de índices
*/
int delete_bin(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, int n, char** fields, char** values, Header *header) ;

/*
 * Configura strings para utilização em update_vehicle, trocando valores nulos 
 * para '\0' a fim de padronizar e utilizar a biblioteca <string.h>
 *  @param:
 *      Vehicle* V: estrutura veículo que terá as strings modificadas
 *  @return:
 *      void
*/
void setup_vehicle_strings(Vehicle *V) ;

/*
 * Atualiza veículo em estrutura V, alterando seus campos e os atualizando
 *  @param:
 *      Vehicle* V: estrutura veículo com os dados a serem atualizados
 *      int n: número de campos a serem atualizados
 *      char** fields: campos a serem atualizados
 *      char** values: valores dos campos a serem atualizados
 *      Index** I_list: lista de índices carregada na memória
 *      int n_indices: número de índices na lista
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      void 
*/
void update_vehicle(Vehicle *V, int n, char** fields, char** values, Index **I_list, int n_indices, int f_type) ;

/*
 * Atualiza veículos no arquivo de dados com os dados presentes em values e campos de fieelds
 *  @param:
 *      FILE* file_bin_rw: arquivo binário de dados
 *      int f_type: tipo do arquivo de dados
 *      Index** I_list: lista de índices carregada na memória
 *      int* n_indices: número de índices na lista
 *      int x:
 *      int** search_fields: campos utilizados na busca
 *      char** search_values: valores dos campos utilizados na busca
 *      char** update_fields: campos a serem atualizados
 *      char** update_values: valores dos campos a serem atualizados
 *      Header* header: cabeçalho do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
*/
int update_bin(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, int x, char** search_fields, char** search_values, int y, char** update_fields, char** update_values, Header *header) ;

/*
 * Busca um registro em um arquivo binário de dados a partir de um 
 * arquivo de índices Árvore-B
 *  @param:
 *      FILE* file_bin_rw: arquivo binário de dados
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      int src_id: id do registro a ser buscado
 *      Header* f_header: cabeçalho do arquivo de dados
 *      B_Header* b_header: cabeçalho da Árvore-B
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu 
 *      1: registro inexistente
*/ 
int search_reg_in_btree(FILE *file_bin_r, FILE *file_btree_r, int src_id, Header *f_header, B_Header *b_header, int f_type);

/*
 * Adiciona um novo registro ao arquivo de dados utilizando uma Árvore-B
 *  @param:
 *      FILE* file_bin_rw: arquivo binário de dados
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      int f_type: tipo do arquivo de dados
 *      Header* f_header: cabeçalho do arquivo de dados
 *      B_Header* b_header: cabeçalho da Árvore-B
 *      char* id: id do novo registro
 *      char* qtt: quantidade do novo registro
 *      char* sigla: sigla do novo registro
 *      char* cidade: cidade do novo registro
 *      char* marca: marca do novo registro
 *      char* modelo: modelo do novo registro
 *  @return:
 *      0: nenhum erro ocorreu
 *      1: registro já existente
*/
int add_new_reg_using_btree(FILE *file_bin_rw, FILE *file_btree_rw, int f_type, Header *f_header, B_Header *b_header, char *id, char *ano, char *qtt, char *sigla, char *cidade, char *marca, char *modelo);


#endif