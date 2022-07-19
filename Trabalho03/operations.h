#ifndef OPERATIONS_H
#define OPERATIONS_H


/*
 * FUNÇÃO FORNECIDA
*/
void binarioNaTela(char *nomeArquivoBinario) ;
 
/*
 * FUNÇÃO FORNECIDA
*/
void scan_quote_string(char *str) ;

/* Operação 1
 * Executa as funções de escrita de registros em um arquivo binário 
 * a partir de um arquivo csv.
*/
void create_table_cmd(int f_type) ;

/* Operação 2
 * Executa as funções de leitura de todos os registros a partir de 
 * um arquivo binário
*/
void select_full_cmd(int f_type) ;

/* Operação 3
 * Executa as funções de leitura de todos os registros a partir de um 
 * arquivo binário que cumpram determinados requisitos de filtragem 
*/
void select_where_cmd(int f_type) ;

/* Operação 4
 * Executa as funções de leitura de um registro a partir de um arquivo
 * binário dado seu RRN 
*/
void select_rrn_cmd() ;

/* Operação 5
 * Create Table 
*/
void create_index_cmd(int f_type) ;

/* Operação 6
 * Delete
*/
void delete_cmd(int f_type) ;

/* Operação 7
 * Insert
*/
void insert_cmd(int f_type) ;

/* Operação 8
 * Update 
*/
void update_cmd(int f_type) ;

/* Operação 9
 * Cria arquivo de índices Árvore-B 
*/
void create_index_btree_cmd(int f_type) ;

/* Operação 10 
 * Seleciona registro a partir do ID buscando pela Árvore-B 
*/
void select_from_btree_cmd(int f_type) ;

/* Operação 11 
 * Adiciona um novo registro utilizando índice Árvore-B
*/
void insert_using_btree_cmd(int f_type) ;


#endif