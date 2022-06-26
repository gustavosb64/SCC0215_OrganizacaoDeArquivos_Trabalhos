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
#include <ctype.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./index.h"

void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

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

/* Operação 5
 * Create Table */
void create_index_cmd(int f_type) {
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_idx = readline(stdin, aux_delimiters);

    write_idx_file_from_bin(f_bin, f_idx, f_type);
    binarioNaTela(f_idx);
    
    free(f_bin);
    free(f_idx);
}

/* Operação 6
 * Delete */
void delete_cmd(int f_type) {
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_idx = readline(stdin, aux_delimiters);

    int n;
    scanf("%d\n", &n);

    // Delimitador utilizado na leitura da string
    char** fields;
    char** values;
    int x;
    for (int i=0; i<n; i++) {
        scanf("%d ", &x);
        values = malloc(x*sizeof(char*));
        fields = malloc(x*sizeof(char*));
        for (int j=0; j<x; j++) {
            fields[j] = readline(stdin, aux_delimiters);
            if (strcmp("id", fields[j]) == 0 || strcmp("ano", fields[j]) == 0 || strcmp("qtt", fields[j]) == 0) {
                values[j] = readline(stdin, aux_delimiters);
            } else {
                values[j] = malloc(30*sizeof(char));
                scan_quote_string(values[j]);
                getchar();
            }
        }

        delete_bin(f_bin, f_type, f_idx, x, fields, values);

        for (int j=0; j<x; j++) {
            free(fields[j]);
            free(values[j]);
        }
        free(fields);
        free(values);
    }
    binarioNaTela(f_bin);
    binarioNaTela(f_idx);

    free(f_bin);
    free(f_idx);
}

/* Operação 7
 * Insert */
void insert_cmd(int f_type) {
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_idx = readline(stdin, aux_delimiters);

    FILE *file_bin_rw = fopen(f_bin, "rb+"); 

    // Setando status para inconsistente
    set_status_bin(file_bin_rw, '0');

    Header *header = read_header_from_bin(file_bin_rw, f_type);

    /*
    printf("-----\n");
    print_header(header, f_type);
    */

    int n;
    scanf("%d\n", &n);

    // Delimitador utilizado na leitura da string
    int id;
    int ano;
    int qtt;
    char* sigla = malloc(3*sizeof(char));
    char* cidade = malloc(30*sizeof(char));
    char* marca = malloc(30*sizeof(char));
    char* modelo = malloc(30*sizeof(char));
    for (int i=0; i<n; i++) {
        scanf("%d %d %d ", &id, &ano, &qtt);
        scan_quote_string(sigla);
        scan_quote_string(cidade);
        scan_quote_string(marca);
        scan_quote_string(modelo);

        //printf("%d, %d, %d, %s, %s, %s, %s\n", id, ano, qtt, sigla, cidade, marca, modelo);

        /*
        printf("##################\n");
        printf("id: %d ano: %d qtt: %d\n",id,ano,qtt);
        printf("##################\n");
        printf("-----\n");
        print_header(header, f_type);
        */

        add_new_reg(file_bin_rw, f_type, header, id, ano, qtt, sigla, cidade, marca, modelo);
//        add_new_reg(file_bin_rw, f_type, id, ano, qtt, sigla, cidade, marca, modelo);
    }

    update_header(file_bin_rw, header, f_type);
    set_status_bin(file_bin_rw, '1');

    fflush(file_bin_rw);
    // Reescrevendo arquivo de índices
    write_idx_file_from_bin(f_bin, f_idx, f_type);

    binarioNaTela(f_bin);
    binarioNaTela(f_idx);
    
    free(sigla);
    free(cidade);
    free(marca);
    free(modelo);
    free(f_bin);
    free(f_idx);

    fclose(file_bin_rw);
}

/* Operação 8
 * Update */
void update_cmd(int f_type) {
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_idx = readline(stdin, aux_delimiters);

    int n;
    scanf("%d\n", &n);

    // Delimitador utilizado na leitura da string
    char** fields;
    char** values;
    int x;
    for (int i=0; i<n; i++) {
        scanf("%d ", &x);
        values = malloc(x*sizeof(char*));
        fields = malloc(x*sizeof(char*));
        for (int j=0; j<x; j++) {
            fields[j] = readline(stdin, aux_delimiters);
            if (strcmp("id", fields[j]) == 0 || strcmp("ano", fields[j]) == 0 || strcmp("qtt", fields[j]) == 0) {
                values[j] = readline(stdin, aux_delimiters);
            } else {
                values[j] = malloc(30*sizeof(char));
                scan_quote_string(values[j]);
                getchar();
            }
        }

        //update_bin(f_bin, f_type, f_idx, x, fields, values);

        for (int j=0; j<x; j++) {
            free(fields[j]);
            free(values[j]);
        }
        free(fields);
        free(values);
    }
    binarioNaTela(f_bin);
    binarioNaTela(f_idx);

    free(f_bin);
    free(f_idx);
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
        
        case 5: {
            create_index_cmd(f_type);
            break;
        }
        
        case 6: {
            delete_cmd(f_type);
            break;
        }
        
        case 7: {
            insert_cmd(f_type);
            break;
        }

        case 8: {
            update_cmd(f_type);
            break;
        }
        
        default: {
            break;
        }
    }

    free(f_type_str);
    return 0;
}
