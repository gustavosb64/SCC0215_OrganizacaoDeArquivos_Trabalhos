#ifndef REG_TYPE1_H
#define REG_TYPE1_H


/*
 * Estrutura contendo os dados do cabeçalho dos arquivos de registro binários
*/
typedef struct header Header;

/*
 * Estrutura contendo os campos dos registros do tipo Vehicle (veículos)
*/
typedef struct vehicle Vehicle;

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
 * Comment section
*/
int remove_reg_by_rrn(FILE *file_bin_rw, int rrn, Header *header);

/*
 * Comment section
*/
int print_reg_from_bin_by_rrn(char *filename, int rrn);

/*
 * Comment section
*/
int add_new_reg_type1(FILE *file_bin_rw, Vehicle V, int *rrn, Header *header);

/*
 * Comment section
*/
int read_id_from_reg_type1(FILE *file_bin_r, int *id, int rrn, Header *header);

/*
 * Comment section
*/
int update_reg_type1(FILE *file_bin_rw, Vehicle V, int rrn);


#endif