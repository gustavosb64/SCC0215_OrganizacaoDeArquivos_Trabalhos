/*
 * Alunos:
 *  Gustavo Siqueira Barbosa, NºUSP 10728122
 *      * porcentagem de participação: 100%
 *  Luiz Fernando Silva Eugênio dos Santos, NºUSP 10892680
 *      * porcentagem de participação: 100%
 * Código do curso: SCC0215
 * Ano/semestre: 2022/1º semestre
 * Título: Terceiro Trabalho Prático
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./index.h"
#include "./b_tree.h"
#include "./operations.h"

int main(int argc, char *argv[]){

    // Lendo os respectivos inputs do stdin
    int operation;
    scanf("%d\n", &operation);
    
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";
    char *f_type_str = readline(stdin, aux_delimiters);
    int f_type = f_type_str[strlen(f_type_str)-1] - '0';

    // Executa as diferentes operações
    switch(operation) {

        // Cria arquivo binário de dados a partir de um csv
        case 1: {
            create_table_cmd(f_type);
            break;
        }

        // Exibe todos os registros em um arquivo de dados
        case 2: {
            select_full_cmd(f_type);
            break;
        }

        // Busca registros com filtro em um arquivo binário de dados
        case 3: {
            select_where_cmd(f_type);
            break;
        }

        // Busca registro especificado por RRN em um arquivo binário de dados
        case 4: {
            select_rrn_cmd();
            break;
        }
        
        // Cria um arquivo de índices a partir de um arquivo binário de dados
        case 5: {
            create_index_cmd(f_type);
            break;
        }
        
        // Deleta um registros de um arquivo de dados utilizando referência por lista de índices
        case 6: {
            delete_cmd(f_type);
            break;
        }
        
        // Insere registros em um arquivo de dados utilizando referência por lista de índices
        case 7: {
            insert_cmd(f_type);
            break;
        }

        // Atualiza um registros em um arquivo de dados utilizando referência por lista de índices
        case 8: {
            update_cmd(f_type);
            break;
        }

        // Cria um arquivo de índices tipo Árvore-B a partir de um arquivo binário de dados
        case 9: {
            create_index_btree_cmd(f_type);
            break;
        }

        // Busca um registro em um arquivo de dados a partir de um arquivo de índices Árvore-B
        case 10: {
            select_from_btree_cmd(f_type);
            break;
        }
        
        // Insere um novo registro no arquivo de dados utilizando um arquivo de índice Árvore-B
        case 11: {
            insert_using_btree_cmd(f_type);
            break;
        }
        
        default: {
            break;
        }
    }

    free(f_type_str);
    return 0;
}
