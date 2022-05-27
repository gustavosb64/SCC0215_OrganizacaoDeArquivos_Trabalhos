/*
 * Alunos:
 *  Gustavo Siqueira Barbosa, NºUSP 10728122
 *  Luiz Fernando Silva Eugênio dos Santos, NºUSP 10892680
 * Código do curso: SCC0215
 * Ano/semestre: 2022/1º semestre
 * Título: Primeiro Trabalho Prático
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./functions.h"

/* Operação 1
 * Executa as funções de escrita de registros em um arquivo binário 
 * a partir de um arquivo csv. */
void create_table_cmd(int f_type) {

    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_input_csv = readline(stdin, aux_delimiters);
    char *f_output_bin = readline(stdin, aux_delimiters);

    // Executa função de escrita
    write_bin_from_csv(f_input_csv, f_output_bin, f_type);
    binarioNaTela(f_output_bin);

    free(f_input_csv);
    free(f_output_bin);
    return;
}

/* Operação 2
 * Executa as funções de leitura de todos os registros a partir de 
 * um arquivo binário */
void select_full_cmd(int f_type) {
    
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";
    char *f_bin = readline(stdin, aux_delimiters);

    // Caso a função retorne sinal de erro 1, exibir mensagem
    if (read_all_reg_from_bin(f_bin, f_type))
        printf("Falha no processamento do arquivo.\n");

    free(f_bin);
}

/* Operação 3
 * Executa as funções de leitura de todos os registros a partir de um 
 * arquivo binário que cumpram determinados requisitos de filtragem */
void select_where_cmd(int f_type) {

    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";
    char *f_bin = readline(stdin, aux_delimiters);

    int n;
    scanf("%d\n", &n);
    char** conditions = malloc(n*sizeof(char*));

    // Delimitador utilizado na leitura da string
    aux_delimiters[0] = '\0';
    char* line;
    for (int i=0; i<n; i++) {
        line = readline(stdin, aux_delimiters);
        conditions[i] = line; //armazena as condições
    }

    // Executa funções de filtragem do arquivo binário
    read_condition_reg_from_bin(f_bin, f_type, conditions, n);

    for (int i=0; i<n; i++) {
        free(conditions[i]);
    }
    free(conditions);
    free(f_bin);
}

/* Operação 4
 * Executa as funções de leitura de um registro a partir de um arquivo
 * binário dado seu RRN */
void select_rrn_cmd() {
    
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";
    char *f_bin = readline(stdin, aux_delimiters);
    
    int rrn;
    scanf("%d", &rrn);
    
    // Exibe diferentes mensagens a depender dos sinais de erro retornados
    switch(search_vehicle_rrn(f_bin, rrn)){
        case 1:
            printf("Falha no processamento do arquivo.\n");
            break;

        case 2:
            printf("Registro inexistente.\n");
            break;

        default:
            break;
    }

    free(f_bin);
}

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
        case 1: {
            create_table_cmd(f_type);
            break;
        }

        case 2: {
            select_full_cmd(f_type);
            break;
        }

        case 3: {
            select_where_cmd(f_type);
            break;
        }

        case 4: {
            select_rrn_cmd();
            break;
        }

        default: {
            break;
        }
    }

    free(f_type_str);
    return 0;
}
