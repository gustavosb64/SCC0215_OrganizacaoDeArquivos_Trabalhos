#ifndef REG_TYPE2_H
#define REG_TYPE2_H


/*
 * Estrutura contendo os dados do cabeçalho dos arquivos de registro binários
*/
typedef struct header Header;

/*
 * Estrutura contendo os campos dos registros do tipo Vehicle (veículos)
*/
typedef struct vehicle Vehicle;

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
 * Remove um registro dado um offset
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      long int *offset: offset referente ao registro a ser removido
 *      Header *header: estrutura Header* com os dados do cabeçalho do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: registro já se encontra removido
*/
int remove_reg_by_offset(FILE *file_bin_rw, long int *offset, Header *header);

/*
 * Limpa um registro tipo 2, armazenando '$' em todos os bytes do registro
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      int tamReg: tamanho do registro que será limpo
 *  @return:
 *      void
*/
void clean_reg_type2(FILE *file_bin_rw, int tamReg);

/*
 * Insere um novo registro tipo 2 no arquivo de dados
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      Vehicle V: estrutura Vehicle com os dados que devem ser escritos no arquivo
 *      Header *header: estrutura Header* com os dados do cabeçalho do arquivo de dados
 *      long int *offset: RRN referente ao registro que será lido
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: registro a ser inserido consta como removido
*/
int add_new_reg_type2(FILE *file_bin_rw, Vehicle V, Header *header, long int *offset);

/*
 * Lê um ID de um registro tipo 2 dado um offset
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      int *id: armazena ID lido de registro
 *      long int *offset: offset referente ao registro que será lido
 *      Header *header: estrutura Header* com os dados do cabeçalho do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
 *      2, 3: falha de leitura
*/
int read_id_from_reg_type2(FILE *file_bin_r, int *id, long int *offset, Header *header);

/*
 * Atualiza um registro tipo 2 com os dados em Vehicle V
 *  @param:
 *      FILE *file_bin_rw: arquivo binário de dados
 *      Vehicle V: estrutura com dados atualizados a serem escritos no arquivo
 *      Header *header: estrutura Header* com os dados do cabeçalho do arquivo de dados
 *      long int *offset: offset referente ao registro que será atualizado
 *      Index **I_list: lista de índices carregada na memória
 *      int n_indices: número de índices em I_list
 *  @return:
 *      0: nenhum erro ocorreu
 *      -1: registro a ser inserido consta como removido
*/
int update_reg_type2(FILE *file_bin_rw, Vehicle V, Header *header, long int *offset, Index **I_list, int n_indices);


#endif
