#ifndef OPERATIONS_H
#define OPERATIONS_H


void binarioNaTela(char *nomeArquivoBinario) ;
 
void scan_quote_string(char *str) ;

void create_table_cmd(int f_type) ;

void select_full_cmd(int f_type) ;

void select_where_cmd(int f_type) ;

void select_rrn_cmd() ;

void create_index_cmd(int f_type) ;

void delete_cmd(int f_type) ;

void insert_cmd(int f_type) ;

void update_cmd(int f_type) ;

void create_index_btree_cmd(int f_type) ;

void select_from_btree_cmd(int f_type) ;

void insert_using_btree_cmd(int f_type) ;


#endif