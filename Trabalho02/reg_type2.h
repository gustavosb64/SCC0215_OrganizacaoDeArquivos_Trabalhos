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
 * Comment section
*/
int read_id_from_reg_type2(FILE *file_bin_r, int *id, long int *offset);


#endif