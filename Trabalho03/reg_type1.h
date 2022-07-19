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
 * Remove um registro tipo 1 dado seu RRN
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      int rrn: RRN referente ao registro a ser removido
 *      Header *header: estrutura Header* com os dados do cabeçalho do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: registro já se encontra removido
*/
int remove_reg_by_rrn(FILE *file_bin_rw, int rrn, Header *header);

/*
 * Imprime na tela um registro tipo 1 dado seu RRN
 *  @param:
 *      char *filename: nome de arquivo a ser lido
 *      int rrn: RRN referente ao registro que será lido
 *  @return:
 *      0: nenhum erro ocorreu
*/
int print_reg_from_bin_by_rrn(char *filename, int rrn);

/*
 * Adiciona um novo registro tipo 1 no arquivo binário de dados
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      Vehicle V: estrutura Vehicle com os dados que devem ser escritos no arquivo
 *      int *rrn: RRN referente ao registro que será lido
 *      Header *header: estrutura Header* com os dados do cabeçalho do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: registro a ser inserido consta como removido
*/
int add_new_reg_type1(FILE *file_bin_rw, Vehicle V, int *rrn, Header *header);

/*
 * Lê um ID de um registro tipo 1 dado seu RRN
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      int *id: armazena ID lido de registro
 *      int *rrn: RRN referente ao registro que será lido
 *      Header *header: estrutura Header* com os dados do cabeçalho do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
 *      2, 3: falha de leitura
*/
int read_id_from_reg_type1(FILE *file_bin_r, int *id, int rrn, Header *header);

/*
 * Atualiza um registro tipo 1 com os dados presentes em Vehicle V
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      Vehicle V: estrutura com dados atualizados a serem escritos no arquivo
 *      int rrn: RRN referente ao registro que será atualizado
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: registro a ser inserido consta como removido
*/
int update_reg_type1(FILE *file_bin_rw, Vehicle V, int rrn);


#endif