#ifndef RECORDS_H
#define RECORDS_H


/*
 * Estrutura contendo os dados do cabeçalho dos arquivos de registro binários
*/
typedef struct header Header;

/*
 * Estrutura contendo os campos dos registros do tipo Vehicle (veículos)
*/
typedef struct vehicle Vehicle;

/*
 * Comment section
*/
char *readline(FILE *stream, char delimiters[]);

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
 *  Retorno:
 *      0: nenhum erro
*/
int write_reg_in_bin_type2(FILE *file_bin_w, Vehicle *V);

/*
 * Lê um registro do tipo 1 com dado RRN de um arquivo binário
 *  Parâmetros:
 *      FILE *file_bin_r: arquivo do qual o registro será lido 
 *      Vehicle *V: registro do tipo Vehicle contendo os dados lidos do
 *                  arquivo fornecido
 *      int rrn: RRN do registro a ser lido
 *  Retorno:
 *      0: nenhum erro
 *      1: erro na leitura do campo ou fim do arquivo atingido
*/
int read_reg_from_bin_type1(FILE *file_bin_r, Vehicle *V, int rrn);

/*
 * Lê um registro do tipo 2 com dado byte offset de um arquivo binário
 *  Parâmetros:
 *      FILE *file_bin_r: arquivo do qual o registro será lido 
 *      Vehicle *V: registro do tipo Vehicle contendo os dados lidos do
 *                  arquivo fornecido
 *      long int offset: byte offset do registro a ser lido
 *  Retorno:
 *      0: nenhum erro
 *      1: erro na leitura do campo ou fim do arquivo atingido
*/
int read_reg_from_bin_type2(FILE *file_bin_r, Vehicle *V, long int *offset);

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
 * Busca um registro no arquivo a partir de um RRN fornecido
 *  Parâmetros:
 *      char *filename_in_bin: nome do arquivo binário de leitura
 *      int rrn: RRN fornecido para ser buscado
 *  Retorno:
 *      0: nenhum erro
 *      1: falha na abertura do arquivo de leitura 
 *      2: falha na leitura do registro (registro inexistente)
*/
int search_vehicle_rrn(char *filename_in_bin ,int rrn) ;

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
int check_meets_condition(Vehicle V, char* field, char* value) ;

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
 * Função fornecida para a comparação dos binários gerados pelas funções de escrita
*/
void binarioNaTela(char *nomeArquivoBinario) ; 


#endif