#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./index.h"
#include "./b_tree.h"

/*
 * FUNÇÃO FORNECIDA
*/
void binarioNaTela(char *nomeArquivoBinario) { 

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

/*
 * FUNÇÃO FORNECIDA
*/
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

    // Caso haja falha na leitura do arquivo, retorna um erro 
    FILE *file_bin_r = fopen(f_bin, "rb");
    if (file_bin_r == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    Header *header = read_header_from_bin(file_bin_r, f_type);

    // Caso arquivo conste como inconsistente, retorna sinal de erro
    if (get_status_from_header(header) != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_idx);
        free(header);
        fclose(file_bin_r);

        return;
    }

    write_idx_file_from_bin(file_bin_r, header, f_idx, f_type);

    fclose(file_bin_r);

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
    
    // Caso haja falha na leitura do arquivo, retorna sinal de erro
    FILE *file_bin_rw = fopen(f_bin, "rb+");
    if (file_bin_rw == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    Header *header = read_header_from_bin(file_bin_rw, f_type);
    
    // Caso arquivo conste como inconsistente, retorna sinal de erro
    if (get_status_from_header(header) != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_idx);
        free(header);
        fclose(file_bin_rw);
    }

    FILE *file_idx_rw = fopen(f_idx, "rb");
    if (file_idx_rw == NULL){
        return;
    }
    int n_indices = 0;
    Index *I_list = load_all_indices_from_idx(file_idx_rw, f_type, &n_indices);

    fclose(file_idx_rw);

    set_status_file(file_bin_rw, '0');

    int n;
    scanf("%d\n", &n);

    char** fields;
    char** values;
    int x;

    for (int i=0; i<n; i++) {
        // Obtendo campos e valores de busca para delete
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

        delete_bin(file_bin_rw, f_type, &I_list, &n_indices, x, fields, values, header);

        for (int j=0; j<x; j++) {
            free(fields[j]);
            free(values[j]);
        }
        free(fields);
        free(values);
        fflush(file_bin_rw);
    }

    // Atualizando cabeçalho 
    update_header(file_bin_rw, header, f_type);
    fclose(file_bin_rw);

    // Atualiza arquivo de índices
    refresh_idx_file(f_idx, I_list, n_indices, f_type);

    binarioNaTela(f_bin);
    binarioNaTela(f_idx);

    free(f_bin);
    free(f_idx);
    free(header);
    free(I_list);
}

/* Operação 7
 * Insert */
void insert_cmd(int f_type) {
    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_idx = readline(stdin, aux_delimiters);

    /* 
     * Abre arquivo de índices e checa seu status
    */ 
    FILE *file_idx_r = fopen(f_idx, "rb"); 
    if (file_idx_r == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Caso arquivo conste como inconsistente, retorna sinal de erro
    char idx_status = get_idx_status(file_idx_r);
    if (idx_status != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_idx);
        fclose(file_idx_r);
    }
    
    // Carrega os índices em uma lista em memória RAM
    int n_indices = 0;
    Index *I_list = load_all_indices_from_idx(file_idx_r, f_type, &n_indices);

    fclose(file_idx_r);

    /* 
     * Abrindo arquivos de dados e checando status
    */
    FILE *file_bin_rw = fopen(f_bin, "rb+"); 
    if (file_bin_rw == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Caso arquivo conste como inconsistente, retorna sinal de erro
    char bin_status = get_status(file_bin_rw);
    if (bin_status != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_idx);
        fclose(file_bin_rw);
    }

    // Setando status de arquivo binário para inconsistente
    set_status_file(file_bin_rw, '0');

    // Carregando cabeçalho na memória RAM
    Header *header = read_header_from_bin(file_bin_rw, f_type);

    int n;
    scanf("%d\n", &n);

    char* id;
    char* ano;
    char* qtt;
    char* sigla = malloc(3*sizeof(char));
    char* cidade = malloc(30*sizeof(char));
    char* marca = malloc(30*sizeof(char));
    char* modelo = malloc(30*sizeof(char));
    for (int i=0; i<n; i++) {
        id = readline(stdin, aux_delimiters);
        ano = readline(stdin, aux_delimiters);
        qtt = readline(stdin, aux_delimiters);
        scan_quote_string(sigla);
        scan_quote_string(cidade);
        scan_quote_string(marca);
        scan_quote_string(modelo);
        getchar();

        add_new_reg(file_bin_rw, f_type, &I_list, &n_indices, header, id, ano, qtt, sigla, cidade, marca, modelo);

    }

    // Atualizando cabeçalho de arquivo binário de dados
    update_header(file_bin_rw, header, f_type);
    fclose(file_bin_rw);

    // Atualiza arquivo de índices
    refresh_idx_file(f_idx, I_list, n_indices, f_type);

    binarioNaTela(f_bin);
    binarioNaTela(f_idx);
    
    free(id);
    free(ano);
    free(qtt);
    free(sigla);
    free(cidade);
    free(marca);
    free(modelo);
    free(f_bin);
    free(f_idx);
    free(header);

    free(I_list);

} 

/* Operação 8
 * Update */
void update_cmd(int f_type) {

    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_idx = readline(stdin, aux_delimiters);
    
    /* 
     * Abre arquivo de índices e checa seu status
    */ 
    FILE *file_idx_r = fopen(f_idx, "rb"); 
    if (file_idx_r == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Caso arquivo conste como inconsistente, retorna sinal de erro
    char idx_status = get_idx_status(file_idx_r);
    if (idx_status != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_idx);
        fclose(file_idx_r);
    }
    
    // Carrega os índices em uma lista em memória RAM
    int n_indices = 0;
    Index *I_list = load_all_indices_from_idx(file_idx_r, f_type, &n_indices);

    fclose(file_idx_r);

    /* 
     * Abrindo arquivos de dados e checando status
    */
    FILE *file_bin_rw = fopen(f_bin, "rb+"); 
    if (file_bin_rw == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Caso arquivo conste como inconsistente, retorna sinal de erro
    char bin_status = get_status(file_bin_rw);
    if (bin_status != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_idx);
        fclose(file_bin_rw);
    }

    // Setando status de arquivo binário para inconsistente
    set_status_file(file_bin_rw, '0');

    // Carregando cabeçalho na memória RAM
    Header *header = read_header_from_bin(file_bin_rw, f_type);

    int n;
    scanf("%d\n", &n);

    char** search_fields;
    char** search_values;
    char** update_fields;
    char** update_values;
    int x,y;
    for (int i=0; i<n; i++) {
        // Obtendo campos e valores para busca
        scanf("%d ", &x);
        search_values = malloc(x*sizeof(char*));
        search_fields = malloc(x*sizeof(char*));
        for (int j=0; j<x; j++) {
            search_fields[j] = readline(stdin, aux_delimiters);
            if (strcmp("id", search_fields[j]) == 0 || strcmp("ano", search_fields[j]) == 0 || strcmp("qtt", search_fields[j]) == 0) {
                search_values[j] = readline(stdin, aux_delimiters);
            } else {
                search_values[j] = malloc(30*sizeof(char));
                scan_quote_string(search_values[j]);
                getchar();
            }
        }

        // Obtendo campos e valore a serem atualizados
        scanf("%d ", &y);
        update_values = malloc(y*sizeof(char*));
        update_fields = malloc(y*sizeof(char*));
        for (int j=0; j<y; j++) {
            update_fields[j] = readline(stdin, aux_delimiters);
            if (strcmp("id", update_fields[j]) == 0 || strcmp("ano", update_fields[j]) == 0 || strcmp("qtt", update_fields[j]) == 0) {
                update_values[j] = readline(stdin, aux_delimiters);
            } else {
                update_values[j] = malloc(30*sizeof(char));
                scan_quote_string(update_values[j]);
                getchar();
            }
        }

        update_bin(file_bin_rw, f_type, &I_list, &n_indices, x, search_fields, search_values, y, update_fields, update_values, header);

        for (int j=0; j<x; j++) {
            free(search_fields[j]);
            free(search_values[j]);
        }
        for (int j=0; j<y; j++) {
            free(update_fields[j]);
            free(update_values[j]);
        }
        free(search_fields);
        free(search_values);
        free(update_fields);
        free(update_values);
        fflush(file_bin_rw);
    }

    // Atualizando cabeçalho 
    update_header(file_bin_rw, header, f_type);
    fclose(file_bin_rw);

    // Reescrevendo arquivo de índices
    refresh_idx_file(f_idx, I_list, n_indices, f_type);

    binarioNaTela(f_bin);
    binarioNaTela(f_idx);

    free(f_bin);
    free(f_idx);
    free(header);
}

/* Operação 9
 * Cria arquivo de índices Árvore-B */
void create_index_btree_cmd(int f_type) {

    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_btree = readline(stdin, aux_delimiters);

    // Caso haja falha na leitura do arquivo, retorna um erro 
    FILE *file_bin_r = fopen(f_bin, "rb");
    if (file_bin_r == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    Header *f_header = read_header_from_bin(file_bin_r, f_type);

    // Caso arquivo conste como inconsistente, retorna sinal de erro
    if (get_status_from_header(f_header) != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_btree);
        free(f_header);
        fclose(file_bin_r);

        return;
    }

    // Cria arquivo de Árvore-B
    write_btree_file_from_bin(file_bin_r, f_header, f_btree, f_type);
    fclose(file_bin_r);

    binarioNaTela(f_btree);
    
    free(f_bin);
    free(f_btree);
    free(f_header);

    return;
}

/* Operação 10 
 * Seleciona registro a partir do ID buscando pela Árvore-B */
void select_from_btree_cmd(int f_type) {

    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_btree = readline(stdin, aux_delimiters);

    /* 
     * ARQUIVO DE DADOS 
    */
    
    // Caso haja falha na leitura do arquivo, retorna um erro 
    FILE *file_bin_r = fopen(f_bin, "rb");
    if (file_bin_r == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    Header *f_header = read_header_from_bin(file_bin_r, f_type);

    // Caso arquivo conste como inconsistente, retorna sinal de erro
    if (get_status_from_header(f_header) != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_btree);
        free(f_header);
        fclose(file_bin_r);

        return;
    }

    /* 
     * ARQUIVO DE ÍNDICES ÁRVORE-B
    */
    FILE *file_btree_r = fopen(f_btree, "rb");
    if (file_btree_r == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    B_Header *b_header = read_header_from_btree(file_btree_r); 

    if (get_status_from_btree_header(b_header) != '1'){
        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_btree);
        free(f_header);
        free(b_header);
        fclose(file_bin_r);
        fclose(file_btree_r);
        
        return;
    }

    // Lê string identificadora "id" da entrada padrão
    char *aux_char = readline(stdin, aux_delimiters);
    free(aux_char);

    // Lê valor do ID 
    int id;
    scanf(" %d",&id);

    // Chama rotina de busca
    search_reg_in_btree(file_bin_r, file_btree_r, id, f_header, b_header, f_type);
    
    fclose(file_bin_r);
    fclose(file_btree_r);
    
    free(f_header);
    free(b_header);
    free(f_bin);
    free(f_btree);

    return;
}

/* Operação 11 
 * Adiciona um novo registro utilizando índice Árvore-B */
void insert_using_btree_cmd(int f_type) {

    // Delimitador utilizado na leitura da string
    char aux_delimiters[1] = " ";

    // Lê nomes dos arquivos
    char *f_bin = readline(stdin, aux_delimiters);
    char *f_btree = readline(stdin, aux_delimiters);

    /*
     * ARQUIVO DE DADOS
    */

    // Caso haja falha na leitura do arquivo, retorna um erro 
    FILE *file_bin_rw = fopen(f_bin, "rb+");
    if (file_bin_rw == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Caso arquivo conste como inconsistente, retorna um erro
    if (get_status(file_bin_rw) != '1'){

        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_btree);
        fclose(file_bin_rw);

        return;
    }

    // Seta status para inconsistente e carrega cabeçalho na memória
    set_status_file(file_bin_rw, '0');
    Header *f_header = read_header_from_bin(file_bin_rw, f_type);

    /*
     * ARQUIVO DE ÍNDICES ÁRVORE-B
    */
    FILE *file_btree_rw = fopen(f_btree, "rb+");
    if (file_btree_rw == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    if (get_status(file_btree_rw) != '1'){
        printf("Falha no processamento do arquivo.");

        free(f_bin);
        free(f_btree);
        free(f_header);
        fclose(file_bin_rw);
        fclose(file_btree_rw);
        
        return;
    }
    set_status_file(file_bin_rw, '0');
    B_Header *b_header = read_header_from_btree(file_btree_rw); 

    int n;
    scanf("%d\n", &n);

    char* id;
    char* ano;
    char* qtt;
    char* sigla = malloc(3*sizeof(char));
    char* cidade = malloc(30*sizeof(char));
    char* marca = malloc(30*sizeof(char));
    char* modelo = malloc(30*sizeof(char));
    for (int i=0; i<n; i++) {
        id = readline(stdin, aux_delimiters);
        ano = readline(stdin, aux_delimiters);
        qtt = readline(stdin, aux_delimiters);
        scan_quote_string(sigla);
        scan_quote_string(cidade);
        scan_quote_string(marca);
        scan_quote_string(modelo);
        getchar();

        add_new_reg_using_btree(file_bin_rw, file_btree_rw, f_type, f_header, b_header, id, ano, qtt, sigla, cidade, marca, modelo);

    }

    // Atualizando cabeçalho de arquivo binário de dados
    update_header(file_bin_rw, f_header, f_type);
    free(f_header);
    fclose(file_bin_rw);

    // Atualizando cabeçalho de arquivo de índices Árvore-B
    write_btree_header(file_btree_rw, b_header, f_type);
    free(b_header);
    fclose(file_btree_rw);

    binarioNaTela(f_bin);
    binarioNaTela(f_btree);

    free(f_bin);
    free(f_btree);
    
    free(id);
    free(ano);
    free(qtt);
    free(sigla);
    free(cidade);
    free(marca);
    free(modelo);
}
