#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./records_utils.h"
#include "./reg_type1.h"
#include "./reg_type2.h"
#include "./index.h"

#define MAX_RRN 97
#define HEADER_SIZE_TYPE1 182
#define HEADER_SIZE_TYPE2 190
struct header{
    char status;        // consistência do arquivo
    union{
        int rrn;            // RRN do último registro logicamente removido (tipo 1)
        long int offset;    // offset do último registro logicamente removido (tipo 2)
    }topo;           
    union{
        int proxRRN;                // próximo RRN disponível
        long int proxByteOffset;    // próximo offset disponível
    }prox;
    int nroRegRem;      // quantidade de registros logicamente removidos
};

struct vehicle{
    char removido;      // indica se o registro está logicamente removido
    int tamanhoRegistro;    // utilizado apenas por registros tipo 2
    union{
        int rrn;           // armazena o RRN do próximo registro (tipo 1)
        long int offset;   // armazena o offset do próximo registro (tipo 2)
    }prox;
    int id;             // código identificador
    int ano;            // ano de fabricação
    int tamCidade;      // tamanho do campo cidade
    char codC5;         // descrição simplificada do campo 5
    char *cidade;       // nome da cidade
    int qtt;            // quantidade de veículos
    char *sigla;        // sigla do estado no qual o veículo está cadastrado
    int tamMarca;       // tamanho do campo marca
    char codC6;         // descrição simplificada do campo 5
    char *marca;        // nome da marca
    int tamModelo;      // tamanho do campo modelo
    char codC7;         // descrição simplificada do campo 5
    char *modelo;       // nome do modelo
};

#define READLINE_BUFFER 4096
char* readline(FILE *stream, char delimiters[]){
    char *string = 0;
    int pos = 0; 

    // Armazena o tamanho do vetor de delimitadores
    int len_delimiters = sizeof((*delimiters));

    // flag para sair do laço while
    char flag = 0;
	do{
        if (pos % READLINE_BUFFER == 0) {
            string = (char *) realloc(string, (pos / READLINE_BUFFER + 1) * READLINE_BUFFER);
        }
        string[pos] = (char) fgetc(stream);

        pos++;

        // Caso o char lido seja um dos delimitadores, flag é marcada
        for(int i=0; i < len_delimiters; i++)
            if (string[pos-1] == delimiters[i]) flag = 1;

    }while(string[pos-1] != '\r' && string[pos-1] != '\n' && !flag && !feof(stream));

    // Caso o último caractere seja \r, o próximo será um \n
    if (string[pos-1] == '\r')
        fgetc(stream);

    string[pos-1] = 0;
    string = (char *) realloc(string, pos);

    return string;
}

// Inicializa dado do tipo Veículo com os valores nulos padrão
Vehicle initialize_vehicle(int f_type){

    Vehicle V;

    V.removido = '0';
    if (f_type == 2){
        V.prox.offset = -1;
        V.tamanhoRegistro = -1;
    }
    else V.prox.rrn = -1; 
    V.id = -1;
    V.ano = -1;
    V.qtt = -1;
    V.sigla = NULL;
    V.tamCidade = 0;
    V.codC5 = -1;
    V.cidade = NULL;
    V.tamMarca = 0;
    V.codC6 = -1;
    V.marca = NULL;
    V.tamModelo = 0;
    V.codC7 = -1;
    V.modelo = NULL;
    
    return V; 
}

int write_header(FILE *file_header_w, int f_type){

    /* Motivos para não se ter utilizado a struct:
     *  Não se pode atribuir diretamente uma string a vetores de caracteres;
     *  porém, é possível declarar o vetor char já inicializado com a string desejada.
     *  Inicializando através da struct, não seria possível atribuir as strings diretamente aos seus componentes */
    char status = '0';

    int topo_t1 = -1;      //usado em arquivo tipo 1
    long int topo_t2 = -1; //usado em arquivo tipo 2
                           
    char descricao[40] = "LISTAGEM DA FROTA DOS VEICULOS NO BRASIL";
    char desC1[22] = "CODIGO IDENTIFICADOR: ";
    char desC2[19] = "ANO DE FABRICACAO: ";
    char desC3[24] = "QUANTIDADE DE VEICULOS: ";
    char desC4[8] = "ESTADO: "; 
    char codC5 = '0';    
    char desC5[16] = "NOME DA CIDADE: ";
    char codC6 = '1';    
    char desC6[18] = "MARCA DO VEICULO: ";
    char codC7 = '2';    
    char desC7[19] = "MODELO DO VEICULO: ";

    int proxRRN = 0;               //usado em arquivo tipo 1 
    long int proxByteOffset = 0;   //usado em arquivo tipo 2

    int nroRegRem = 0;      

    // Escreve os dados no arquivo
    fwrite(&status, sizeof(char), 1, file_header_w);
    if (f_type == 1) fwrite(&topo_t1, sizeof(int), 1, file_header_w);
    else fwrite(&topo_t2, sizeof(long int), 1, file_header_w);
    fwrite(descricao, sizeof(char), 40, file_header_w);
    fwrite(&desC1, sizeof(char), 22, file_header_w);
    fwrite(&desC2, sizeof(char), 19, file_header_w);
    fwrite(&desC3, sizeof(char), 24, file_header_w);
    fwrite(&desC4, sizeof(char), 8, file_header_w);
    fwrite(&codC5, sizeof(char), 1, file_header_w);
    fwrite(&desC5, sizeof(char), 16, file_header_w);
    fwrite(&codC6, sizeof(char), 1, file_header_w);
    fwrite(&desC6, sizeof(char), 18, file_header_w);
    fwrite(&codC7, sizeof(char), 1, file_header_w);
    fwrite(&desC7, sizeof(char), 19, file_header_w);
    if (f_type == 1) fwrite(&proxRRN, sizeof(int), 1, file_header_w);
    else fwrite(&proxByteOffset, sizeof(long int), 1, file_header_w);
    fwrite(&nroRegRem, sizeof(int), 1, file_header_w);

    return 0;
}

Header* initialize_header(int f_type){

    // Inicializa um header para utilizá-lo na memória durante o programa
    Header *H = (Header *) malloc(sizeof(Header));

    H->status = '0';
    if (f_type == 1) H->topo.rrn = -1;
    else if (f_type == 2) H->topo.offset = -1;

    if (f_type == 1) H->prox.proxRRN = 0;
    else if (f_type == 2) H->prox.proxByteOffset = 0;

    H->nroRegRem = 0;

    return H;
}

Header* read_header_from_bin(FILE *file_bin_r, int f_type){

    // Inicializa um header 
    Header *H = initialize_header(f_type); 

    // Guarda o offset atual
    long int cur_offset = ftell(file_bin_r);
    fseek(file_bin_r, 0, SEEK_SET);

    // Lê dados do header
    fread(&(H->status), sizeof(char), 1, file_bin_r);
    if (f_type == 1) fread(&(H->topo.rrn), sizeof(int), 1, file_bin_r);
    else if (f_type == 2) fread(&(H->topo.offset), sizeof(long int), 1, file_bin_r);

    int size;
    if (f_type == 1){

        size = HEADER_SIZE_TYPE1 - sizeof(int) - sizeof(int);
        fseek(file_bin_r, size, SEEK_SET);

        fread(&(H->prox.proxRRN), sizeof(int), 1, file_bin_r);
    }
    else if (f_type == 2){

        size = HEADER_SIZE_TYPE2 - sizeof(int) - sizeof(long int);
        fseek(file_bin_r, size, SEEK_SET);

        fread(&(H->prox.proxByteOffset), sizeof(long int), 1, file_bin_r);
    }

    fread(&(H->nroRegRem), sizeof(int), 1, file_bin_r);

    fseek(file_bin_r, cur_offset, SEEK_SET);
    
    return H;
}

int update_header(FILE *file_bin_rw, Header *H, int f_type){

    fseek(file_bin_rw, 0, SEEK_SET);

    // Escrevendo status
    H->status = '1';
    fwrite(&(H->status), sizeof(char), 1, file_bin_rw);

    if (f_type == 1) fwrite(&(H->topo.rrn), sizeof(int), 1, file_bin_rw);
    else if (f_type == 2) fwrite(&(H->topo.offset), sizeof(long int), 1, file_bin_rw);

    int size;
    if (f_type == 1){

        size = HEADER_SIZE_TYPE1 - sizeof(int) - sizeof(int);
        fseek(file_bin_rw, size, SEEK_SET);

        fwrite(&(H->prox.proxRRN), sizeof(int), 1, file_bin_rw);
    }
    else if (f_type == 2){

        size = HEADER_SIZE_TYPE2 - sizeof(int) - sizeof(long int);
        fseek(file_bin_rw, size, SEEK_SET);

        fwrite(&(H->prox.proxByteOffset), sizeof(long int), 1, file_bin_rw);
    }

    fwrite(&(H->nroRegRem), sizeof(int), 1, file_bin_rw);

    return 0;
}

void print_header(Header *H, int f_type){

    printf("status: %c\n",H->status);

    if (f_type == 1) printf("topo: %d\n", H->topo.rrn);
    else printf("topo: %ld\n", H->topo.offset);

    if (f_type == 1) printf("prox: %d\n", H->prox.proxRRN);
    else printf("prox: %ld\n", H->prox.proxByteOffset);

    printf("nroRegRem: %d\n",H->nroRegRem);
    
    return;
}

int read_all_reg_from_bin(char *filename_in_bin, int f_type){

    // Caso haja falha na leitura do arquivo, retorna 1
    FILE *file_bin_r = fopen(filename_in_bin, "rb");
    if (file_bin_r == NULL){
        return 1;
    }

    Vehicle V = initialize_vehicle(f_type);

    // Realiza diferentes rotinas a depender do tipo a ser lido
    if (f_type == 1){

        int rrn = 0;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_reg_from_bin_type1(file_bin_r, &V, rrn)){

            // Imprime os dados do veículo
            //print_vehicle(V,1);
            print_vehicle_full(V,1);
            printf("\n");

            // Libera a memória alocada durante a leitura
            free_vehicle(&V);
            V = initialize_vehicle(1);
            rrn++;
        }

    }
    else if (f_type == 2){

        long int offset = HEADER_SIZE_TYPE2;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_reg_from_bin_type2(file_bin_r, &V, &offset)){

            // Imprime os dados do veículo
            print_vehicle(V, 2);
            printf("\n");

            // Libera a memória alocada durante a leitura
            free_vehicle(&V);
            V = initialize_vehicle(2);
        }

    }

    fclose(file_bin_r);

    return 0;
}


int read_reg_from_csv(FILE *file_csv_r, Vehicle *V){

    // Delimitador a ser usado na leitura das strings do csv
    char aux_delimiters[1] = ",";

    // Caso nada seja lido no ID, não há mais dados no arquivo
    char *str_id = readline(file_csv_r, aux_delimiters);
    if (str_id[0] == 0){
        free(str_id);
        return 1;
    }

    // Lendo corretamente os dados do arquivo csv
    int id = atoi(str_id);
    char *str_ano = readline(file_csv_r, aux_delimiters);
    int ano = atoi(str_ano); 
    char *cidade = readline(file_csv_r, aux_delimiters);
    char *str_qtt = readline(file_csv_r, aux_delimiters);
    int qtt = atoi(str_qtt);
    char *sigla = readline(file_csv_r, aux_delimiters);
    char *marca = readline(file_csv_r, aux_delimiters); 
    char *modelo = readline(file_csv_r, aux_delimiters);

    // Armazenando os dados lidos no registro, quando presentes
    if (id) (*V).id = id;
    if (ano) (*V).ano = ano;
    if (cidade[0]) (*V).cidade = cidade;
    else free(cidade);
    if (qtt) (*V).qtt = qtt;
    if (sigla[0]) (*V).sigla = sigla;
    else free(sigla);
    if (marca[0]) (*V).marca = marca;
    else free(marca);
    if (modelo[0]) (*V).modelo = modelo;
    else free(modelo);

    // Liberando memória das strings auxiliares utilizadas
    free(str_id);
    free(str_ano);
    free(str_qtt);

    return 0;
}

int write_bin_from_csv(char *filename_in_csv, char *filename_out_bin, int f_type){

    // Caso haja falha na leitura do arquivo, retorna 1
    FILE *file_csv_r = fopen(filename_in_csv, "r");
    if (file_csv_r == NULL){
        return 1;
    }
    FILE *file_bin_w = fopen(filename_out_bin, "wb");

    write_header(file_bin_w, f_type);
    
    // Lendo e liberando linha de cabeçalho
    char aux_delimiters[1] = "\0";
    free(readline(file_csv_r, aux_delimiters));

    Vehicle V = initialize_vehicle(f_type);

    // Realiza diferentes rotinas a depender do tipo do arquivo
    if (f_type == 1){

        // Contador de RRN, usado para marcar o próximo RRN no cabeçalho
        int rrn_counter = 0;

        // Enquanto ainda houverem dados a serem lidos
        while(!read_reg_from_csv(file_csv_r, &V)){

            // Escrevendo o registro 
            write_reg_in_bin_type1(file_bin_w, &V);

            // Preparando um novo registro a ser escrito
            free_vehicle(&V);
            V = initialize_vehicle(1);

            rrn_counter++; 
        }

        // Atualizando proxRRN no cabeçalho
        update_prox(file_bin_w, 1, rrn_counter);

    }
    else if (f_type == 2){

        while(!read_reg_from_csv(file_csv_r, &V)){

            // Escrevendo o registro 
            write_reg_in_bin_type2(file_bin_w, &V);

            // Preparando um novo registro a ser escrito
            free_vehicle(&V);
            V = initialize_vehicle(2);
        }

        // Atualizando proxByteOffset no cabeçalho
        long int cur_prox_offset = ftell(file_bin_w);
        update_prox(file_bin_w, 2, cur_prox_offset);
    }
    
    set_status_bin(file_bin_w, '1');

    // Liberando memória
    fclose(file_bin_w);
    fclose(file_csv_r);

    return 0;
}

char* remove_quotes_str(char* quoted_str) {
    char* unquoted_str = calloc(1, strlen(quoted_str)-1);

    for (int i=1; i<strlen(quoted_str)-1; i++) {
        unquoted_str[i-1] = quoted_str[i];
    }
    unquoted_str[strlen(quoted_str)-2] = '\0';

    return unquoted_str;
}

int customized_strcmp(char *v_str, char *str){

    // Adiciona '\0' ao final da string v_str para a possibilitar o uso do strcmp
    v_str[strlen(str)] = '\0';
    return strcmp(str, v_str);

}

int check_meets_condition(Vehicle V, char* field, char* value, int quoted) {

    char* unquoted_value;
    if (strcmp(field, "id") == 0) {
        if(V.id == atoi(value)) return 1;
    } else if (strcmp(field, "marca") == 0) {
        if(quoted)
            unquoted_value = remove_quotes_str(value);
        else
            unquoted_value = value;
        if (V.marca != NULL && customized_strcmp(V.marca, unquoted_value) == 0){
            if(quoted)
                free(unquoted_value);
            return 1;
        }
        if(quoted)
            free(unquoted_value);
    } else if (strcmp(field, "cidade") == 0) {
        if(quoted)
            unquoted_value = remove_quotes_str(value);
        else
            unquoted_value = value;
        if (V.cidade != NULL && customized_strcmp(V.cidade, unquoted_value) == 0){
            if(quoted)
                free(unquoted_value);
            return 1;
        }
        if(quoted)
            free(unquoted_value);
    } else if (strcmp(field, "estado") == 0 || strcmp(field, "sigla") == 0) {
        if(quoted)
            unquoted_value = remove_quotes_str(value);
        else 
            unquoted_value = value;
        if (V.sigla != NULL && customized_strcmp(V.sigla, unquoted_value) == 0){
            if(quoted)
                free(unquoted_value);
            return 1;
        }
        if(quoted)
            free(unquoted_value);
    } else if (strcmp(field, "modelo") == 0) {
        if(quoted)
            unquoted_value = remove_quotes_str(value);
        else
            unquoted_value = value;
        if (V.modelo != NULL && customized_strcmp(V.modelo, unquoted_value) == 0) {
            if(quoted)
                free(unquoted_value);
            return 1;
        }
        if(quoted)
            free(unquoted_value);
    } else if (strcmp(field, "quantidade") == 0) {
        if(V.qtt == atoi(value)) return 1;
    } else if (strcmp(field, "ano") == 0) {
        if(V.ano == atoi(value)) return 1;
    }

    return 0;
}

int read_condition_reg_from_bin(char *filename_in_bin, int f_type, char** conditions, int n){
    char** fields = malloc(n*sizeof(char*));
    char** values = malloc(n*sizeof(char*));
    char* sp;
    for (int i=0; i<n; i++) {
        sp = strchr(conditions[i], ' ');
        fields[i] = strndup(conditions[i], sp-conditions[i]);;
        values[i] = sp+1;
    }

    // Caso haja falha na leitura do arquivo, retorna 1
    FILE *file_bin_r = fopen(filename_in_bin, "rb");
    if (file_bin_r == NULL){
        return 1;
    }

    Vehicle V = initialize_vehicle(f_type);
    int is_selected;

    // Realiza diferentes rotinas a depender do tipo a ser lido
    if (f_type == 1){

        int rrn = 0;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_reg_from_bin_type1(file_bin_r, &V, rrn)){

            // Checa se atende à todas as condições do select
            is_selected = 0;
            for (int i=0; i<n; i++) {
                is_selected = is_selected + check_meets_condition(V, fields[i], values[i], 1);
            }        
            if (is_selected == n) {
                // Imprime os dados do veículo
                print_vehicle(V,1);
                printf("\n");
            }

            // Libera a memória alocada durante a leitura
            free_vehicle(&V);
            V = initialize_vehicle(1);
            rrn++;
        }

    }
    else if (f_type == 2){

        long int offset = HEADER_SIZE_TYPE2;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_reg_from_bin_type2(file_bin_r, &V, &offset)){

            // Checa se atende à todas as condições do select
            is_selected = 0;
            for (int i=0; i<n; i++) {
                is_selected = is_selected + check_meets_condition(V, fields[i], values[i], 1);
            }        
            if (is_selected == n) {
                // Imprime os dados do veículo
                print_vehicle(V,2);
                printf("\n");
            }

            // Libera a memória alocada durante a leitura
            free_vehicle(&V);
            V = initialize_vehicle(2);
        }

    }

    for (int i=0; i<n; i++) { 
        free(fields[i]);
    }
    free(fields);
    free(values);
    fclose(file_bin_r);

    return 0;
}

int print_string(char string[], int len){

    // Conferindo se comprimento fornecido é maior que 0
    if (len <= 0){
        return 1;
    }

    // Imprimindo na saída padrão os caracteres da string
    for (int i=0; i<len; i++)
        printf("%c",string[i]);

    return 0;
}

int print_vehicle_full(Vehicle V, int f_type){

    printf("Removido: %d", V.removido);
    if (f_type == 2) printf("\nTamanhoRegistro: %d", V.tamanhoRegistro);
    if (f_type == 1) printf("\nPróximo RRN: %d", V.prox.rrn);
    else printf("\nPróximo offset: %ld", V.prox.offset);
    printf("\nID: %d", V.id);             
    printf("\nAno de fabricação: %d", V.ano);            
    printf("\nQuantidade de carros: %d", V.qtt);            
    printf("\nEstado: ");
    if (V.sigla != NULL) print_string(V.sigla, 2);
    printf("\ntamCidade: %d", V.tamCidade);      
    printf("\ncodC5: %c", V.codC5);         
    printf("\nNome da cidade: ");
    if (V.cidade != NULL) print_string(V.cidade, V.tamCidade);
    printf("\ntamMarca: %d", V.tamMarca);       
    printf("\ncodC6: %c", V.codC6);         
    printf("\nNome da marca: ");
    if (V.marca != NULL) print_string(V.marca, V.tamMarca);
    printf("\ntamModelo: %d", V.tamModelo);      
    printf("\ncodC7: %c", V.codC7);         
    printf("\nNome do modelo: ");
    if (V.modelo != NULL) print_string(V.modelo, V.tamModelo);
    printf("\n");
    
    return 0;
}

int print_vehicle(Vehicle V, int f_type){
           
    printf("MARCA DO VEICULO: ");
    if (V.marca != NULL) print_string(V.marca, V.tamMarca); else printf("NAO PREENCHIDO"); 

    printf("\nMODELO DO VEICULO: ");
    if (V.modelo != NULL) print_string(V.modelo, V.tamModelo); else printf("NAO PREENCHIDO"); 

    printf("\nANO DE FABRICACAO: ");          
    if (V.ano != -1) printf("%d", V.ano); else printf("NAO PREENCHIDO"); 

    printf("\nNOME DA CIDADE: ");
    if (V.cidade != NULL) print_string(V.cidade, V.tamCidade); else printf("NAO PREENCHIDO"); 

    printf("\nQUANTIDADE DE VEICULOS: ");
    if (V.qtt != -1) printf("%d", V.qtt); else printf("NAO PREENCHIDO"); 

    printf("\n");
    
    return 0;
}

int free_vehicle(Vehicle *V){

    free((*V).sigla);
    if ((*V).tamCidade) free((*V).cidade);
    if ((*V).tamMarca) free((*V).marca);
    if ((*V).tamModelo) free((*V).modelo);

    return 0;
}

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

int add_new_reg(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, Header *header, char *id, char *ano, char *qtt, char *sigla, char *cidade, char *marca, char *modelo){

    Vehicle V = initialize_vehicle(f_type);

    // Prepara os dados do novo veículo para inseri-lo no arquivo
    V.tamCidade = strlen(cidade);
    V.tamMarca = strlen(marca);
    V.tamModelo = strlen(modelo);

    V.id = atoi(id);
    if (strcmp("NULO", ano)){
        V.ano = atoi(ano);
    }
    else V.ano = -1;
    if (strcmp("NULO", qtt)){
        V.qtt = atoi(qtt);
    }
    else V.qtt = -1;
    if (strlen(sigla) == 0){
        V.sigla = NULL;
    }
    else V.sigla = sigla;
    V.cidade = cidade;
    V.marca = marca;
    V.modelo = modelo;

    if (f_type == 1){
        int rrn;
        add_new_reg_type1(file_bin_rw, V, &rrn, header);
        add_new_index(I_list, n_indices, V.id, rrn, f_type);
    }
    else if (f_type == 2){

        V.tamanhoRegistro = V.tamCidade+5 + V.tamMarca+5 + V.tamModelo+5 + 3*sizeof(int) + sizeof(long int) + 2;

        long int offset = 0;
        add_new_reg_type2(file_bin_rw, V, header, &offset);

        add_new_index(I_list, n_indices, V.id, offset, f_type);

    }

    return 0;
}

// operation = '+' ou '-'
int update_nroRegRem(FILE *file_bin_rw, int f_type, char operation){

    long int cur_offset = ftell(file_bin_rw);
    int offset;

    // Ajustando offset para o nroRegRem
    if (f_type == 1) 
        offset = HEADER_SIZE_TYPE1 - sizeof(int); 
    else if (f_type == 2) 
        offset = HEADER_SIZE_TYPE2 - sizeof(int); 
    else 
        return -1;
    fseek(file_bin_rw, offset, SEEK_SET);

    // Lê nroRegRem atual
    int nroRegRem;
    fread(&nroRegRem, sizeof(int), 1, file_bin_rw);
    
    // Decrementa ou incrementa de acordo com a opção do usuário
    if (operation == '+'){
        nroRegRem += 1;
    }
    else if (operation == '-'){
        nroRegRem -= 1;
    }
    else{
        // Caso a operação enviada seja diferente de '+' ou '-'
        printf("Operação '%c' enviada para update_nroRegRem inválida.\n", operation);

        // Retorna o ponteiro para a posição que estava no início da função
        fseek(file_bin_rw, cur_offset, SEEK_SET);
        return -2;
    }

    // Escreve novo nroRegRem
    fseek(file_bin_rw, -sizeof(int), SEEK_CUR);
    fwrite(&nroRegRem, sizeof(int), 1, file_bin_rw);

    // Retorna o ponteiro para a posição que estava no início da função
    fseek(file_bin_rw, cur_offset, SEEK_SET);

    return 0;
}

int update_list(FILE *file_bin_rw, int f_type, long int new_value){

    long int cur_offset = ftell(file_bin_rw);
    int offset, size;

    offset = sizeof(char);
    if (f_type == 1) 
        size = sizeof(int);
    else if (f_type == 2) 
        size = sizeof(long int);
    else{
        // Retorna o ponteiro para a posição que estava no início da função
        fseek(file_bin_rw, cur_offset, SEEK_SET);
        return -1;
    }

    // Atualiza o cabeçalho com o novo topo da pilha
    fseek(file_bin_rw, offset, SEEK_SET);
    fwrite(&new_value, size, 1, file_bin_rw);

    // Retorna o ponteiro para a posição que estava no início da função
    fseek(file_bin_rw, cur_offset, SEEK_SET);

    return 0;
}

int update_prox(FILE *file_bin_rw, int f_type, long int new_value){

    long int cur_offset = ftell(file_bin_rw);
    int offset, size;

    if (f_type == 1){ 
        size = sizeof(int);
        int end_offset = 2*sizeof(int);
        offset = HEADER_SIZE_TYPE1 - end_offset;
    }
    else if (f_type == 2){
        size = sizeof(long int);
        int end_offset = sizeof(int) + sizeof(long int);
        offset = HEADER_SIZE_TYPE2 - end_offset;
    }
    else {
        // Retorna o ponteiro para a posição que estava no início da função
        fseek(file_bin_rw, cur_offset, SEEK_SET);

        return -1;
    }

    // Atualiza o cabeçalho com o prox rrn ou byteoffset disponível
    fseek(file_bin_rw, offset, SEEK_SET);
    fwrite(&new_value, size, 1, file_bin_rw);

    // Retorna o ponteiro para a posição que estava no início da função
    fseek(file_bin_rw, cur_offset, SEEK_SET);

    return 0;
}

long int get_list_top(FILE *file_bin_rw, int f_type){

    int offset;
    long int top;

    offset = sizeof(char);
    fseek(file_bin_rw, offset, SEEK_SET);

    if (f_type == 1) fread(&top, sizeof(int), 1, file_bin_rw); 
    else if (f_type == 2) fread(&top, sizeof(long int), 1, file_bin_rw); 

    return top;
}

long int get_prox(FILE *file_bin_rw, int f_type){

    int offset;
    long int prox;

    if (f_type == 1){
        offset = HEADER_SIZE_TYPE1 - 2*sizeof(int); 

        fseek(file_bin_rw, offset, SEEK_SET);
        fread(&prox, sizeof(int), 1, file_bin_rw); 
    }
    else if (f_type == 2){
        offset = HEADER_SIZE_TYPE2 - sizeof(int) - sizeof(long int); 

        fseek(file_bin_rw, offset, SEEK_SET);
        fread(&prox, sizeof(long int), 1, file_bin_rw); 
    }
    else return -1;

    return prox;
}

int set_status_bin(FILE *file_bin_rw, char status){

    if (status != '0' && status != '1')
        return -1;

    long int cur_offset = ftell(file_bin_rw);

    fseek(file_bin_rw, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file_bin_rw);

    fseek(file_bin_rw, cur_offset, SEEK_SET);

    return 0;
}

char get_status(FILE *file_bin_r){
    int status;
    fseek(file_bin_r, 0, SEEK_SET);
    fread(&status, 1, sizeof(char), file_bin_r);

    return status;
}

int delete_bin(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, int n, char** fields, char** values, Header *header) {

    int has_id = 0;
    int is_selected;
    for(int i=0; i<n; i++) {
        /*
        *   CAMPO id PRESENTE NA BUSCA
        *   Recuperção do vehicle por id e checagem de demais valores
        */
        if(strcmp("id", fields[i])==0) {
            // Busca por id no arquivo de índice
            has_id = 1;

            Vehicle V = initialize_vehicle(f_type);

            // Arquivo tipo 1
            if (f_type==1) {
                int rrn = search_index_from_idx((*I_list), (*n_indices), atoi(values[i]), f_type);
                
                // Caso o registro não seja encontrado
                if (rrn == -1) return -1;

                read_reg_from_bin_type1(file_bin_rw, &V, rrn);

                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int j=0; j<n; j++) {
                    if (strcmp("id", fields[i])!=0) 
                        is_selected = is_selected + check_meets_condition(V, fields[j], values[j], 0);
                }  
                if (is_selected == n-1) {

                    // Executa remoção
                    if (rrn >= 0)
                        remove_reg_by_rrn(file_bin_rw, rrn, header);

                    // Remove do arquivo de índices
                    remove_index(I_list, n_indices, V.id);
                }

            // Arquivo tipo 2
            } else if(f_type==2) {
                long int offset = search_index_from_idx((*I_list), (*n_indices), atoi(values[i]), f_type);

                // Caso o registro não seja encontrado
                if (offset == -1) return -1;

                read_reg_from_bin_type2(file_bin_rw, &V, &offset);

                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int j=0; j<n; j++) {
                    if (strcmp("id", fields[i])!=0) 
                        is_selected = is_selected + check_meets_condition(V, fields[j], values[j], 0);
                }  
                if (is_selected == n-1) {
                    
                    // Retorna o ponteiro do arquivo para o registro selecionado
                    long int rem_offset = offset - V.tamanhoRegistro - 5;

                    // Executa remoção
                    if (rem_offset >= 0)
                        remove_reg_by_offset(file_bin_rw, &rem_offset, header);

                    // Remove do arquivo de índices
                    remove_index(I_list, n_indices, V.id);
                }
            }
            free_vehicle(&V);
            
        } 
     }

    /*
    *   AUSÊNCIA DO CAMPO id
    *   Busca como a do 'select where'
    */
    if(!has_id) {
        // Busca por valores nos campos
        Vehicle V = initialize_vehicle(f_type);

        // Realiza diferentes rotinas a depender do tipo a ser lido

        // Arquivo tipo 1
        if (f_type == 1){

            int rrn = 0;

            // Enquanto ainda houverem registros a serem lidos no arquivo de dados
            while(!read_reg_from_bin_type1(file_bin_rw, &V, rrn)){
                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int i=0; i<n; i++) {
                    if (strcmp("id", fields[i])!=0) 
                        is_selected = is_selected + check_meets_condition(V, fields[i], values[i], 0);
                }        

                if (is_selected == n) {
                    
                    // Executa remoção
                    if (rrn >= 0)
                        remove_reg_by_rrn(file_bin_rw, rrn, header);
                    
                    // Remove do arquivo de índices
                    remove_index(I_list, n_indices, V.id);

                }

                // Libera a memória alocada durante a leitura
                free_vehicle(&V);
                V = initialize_vehicle(1);
                rrn++;
            }

        }
        // Arquivo tipo 2
        else if (f_type == 2){

            long int offset = HEADER_SIZE_TYPE2;
            long int rem_offset;

            // Enquanto ainda houverem registros a serem lidos no arquivo de dados
            while(!read_reg_from_bin_type2(file_bin_rw, &V, &offset)){
                
                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int i=0; i<n; i++) {
                    if (strcmp("id", fields[i])!=0) 
                        is_selected = is_selected + check_meets_condition(V, fields[i], values[i], 0);
                }        
                if (is_selected == n) {

                    // Retorna o ponteiro do arquivo para o registro selecionado
                    rem_offset = offset - V.tamanhoRegistro - 5;

                    // Executa remoção
                    if (rem_offset >= 0)
                        remove_reg_by_offset(file_bin_rw, &rem_offset, header);
                    
                    // Remove do arquivo de índices
                    remove_index(I_list, n_indices, V.id);
                }

                // Libera a memória alocada durante a leitura
                free_vehicle(&V);
                V = initialize_vehicle(2);
            }
        }
    }

    return 0;
}

/*
*   Tratamento adicional de strings do veículo lido para atualização
*/
void setup_vehicle_strings(Vehicle *V) {
    if(V->marca != NULL)
        V->marca[V->tamMarca] = '\0';
    if(V->cidade != NULL)
        V->cidade[V->tamCidade] = '\0';
    if(V->modelo!= NULL)
        V->modelo[V->tamModelo] = '\0';
    return;
}

/*
*   Altera campos especificados do veículo
*/
void update_vehicle(Vehicle *V, int n, char** fields, char** values) {
    for(int i=0; i<n; i++){
        if (strcmp(fields[i], "id") == 0) {
            V->id = atoi(values[i]);

            } else if (strcmp(fields[i], "marca") == 0) {
                if(V->marca != NULL)
                    free(V->marca);

                if(strlen(values[i])!=0) {
                    V->marca = (char *) calloc(strlen(values[i])+1, sizeof(char));
                    strcpy(V->marca, values[i]);
                } else {
                    V->marca = NULL;
                    V->tamMarca = 0;
                }

            } else if (strcmp(fields[i], "cidade") == 0) {
                if(V->cidade != NULL)
                    free(V->cidade);

                if(strlen(values[i])!=0) {
                    V->cidade = (char *) calloc(strlen(values[i])+1, sizeof(char));
                    strcpy(V->cidade, values[i]);
                } else {
                    V->cidade = NULL;
                    V->tamCidade = 0;
                }

            } else if (strcmp(fields[i], "estado") == 0 || strcmp(fields[i], "sigla") == 0) {
                if(V->sigla != NULL)
                    free(V->sigla);

                if(strlen(values[i])!=0) {
                    V->sigla = (char *) calloc(strlen(values[i])+1, sizeof(char));
                    strcpy(V->sigla, values[i]);
                } else {
                    V->sigla = NULL;
                }

            } else if (strcmp(fields[i], "modelo") == 0) {
                if(V->modelo != NULL)
                    free(V->modelo);

                if(strlen(values[i])!=0) {
                    V->modelo = (char *) calloc(strlen(values[i])+1, sizeof(char));
                    strcpy(V->modelo, values[i]);
                } else {
                    V->modelo = NULL;
                    V->tamModelo = 0;
                }

            } else if (strcmp(fields[i], "qtt") == 0) {
                if(strcmp(values[i], "NULO")!=0)
                    V->qtt = atoi(values[i]);
                else
                    V->qtt = -1;

            } else if (strcmp(fields[i], "ano") == 0) {
                if(strcmp(values[i], "NULO")!=0)
                    V->ano = atoi(values[i]);
                else
                    V->ano = -1;
            }
    }
    setup_vehicle_strings(V);
    return;
}

int update_bin(FILE *file_bin_rw, int f_type, Index **I_list, int *n_indices, int x, char** search_fields, char** search_values, int y, char** update_fields, char** update_values, Header *header) {
    int has_id = 0;
    int is_selected;
    for(int i=0; i<x; i++) {
        /*
        *   CAMPO id PRESENTE NA BUSCA
        *   Recuperção do vehicle por id e checagem de demais valores
        */
        if(strcmp("id", search_fields[i])==0) {
            // Busca por id no arquivo de índice
            has_id = 1;

            Vehicle V = initialize_vehicle(f_type);
            
            // Arquivo tipo 1
            if (f_type==1) {
                int rrn = search_index_from_idx((*I_list), (*n_indices), atoi(search_values[i]), f_type);
                read_reg_from_bin_type1(file_bin_rw, &V, rrn);

                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int j=0; j<x; j++) {
                    if (strcmp("id", search_fields[j])!=0)
                        is_selected = is_selected + check_meets_condition(V, search_fields[j], search_values[j], 0);
                }  
                if (is_selected == x-1) {
                    // Executa update
                    if (rrn >= 0) {
                        update_vehicle(&V, y, update_fields, update_values);
                        long int file_offset = (rrn)*MAX_RRN + HEADER_SIZE_TYPE1;
                        fseek(file_bin_rw, file_offset, SEEK_SET);

                        write_reg_in_bin_type1(file_bin_rw, &V);
                    }

                }
            
            // Arquivo tipo 2
            } else if(f_type==2) {
                long int offset = search_index_from_idx((*I_list), (*n_indices), atoi(search_values[i]), f_type);
                //printf("%ld\n", offset);
                read_reg_from_bin_type2(file_bin_rw, &V, &offset);
                long int up_offset = offset - V.tamanhoRegistro - 5;

                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int j=0; j<x; j++) {
                    if (strcmp("id", search_fields[j])!=0) 
                        is_selected = is_selected + check_meets_condition(V, search_fields[j], search_values[j], 0);
                }  
                if (is_selected == x-1) {
                    // Executa update
                    update_vehicle(&V, y, update_fields, update_values);
                    update_reg_type2(file_bin_rw, V, header, &up_offset, I_list, (*n_indices));
                }
            }
            free_vehicle(&V);
        } 
     }

    /*
    *   AUSÊNCIA DO CAMPO id
    *   Busca como a do 'select where'
    */
    if(!has_id) {
        // Busca por valores nos campos
        Vehicle V = initialize_vehicle(f_type);

        // Realiza diferentes rotinas a depender do tipo a ser lido

        // Arquivo tipo 1
        if (f_type == 1){

            int rrn = 0;

            // Enquanto ainda houverem registros a serem lidos no arquivo de dados
            while(!read_reg_from_bin_type1(file_bin_rw, &V, rrn)){
                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int i=0; i<x; i++) {
                    if (strcmp("id", search_fields[i])!=0) 
                        is_selected = is_selected + check_meets_condition(V, search_fields[i], search_values[i], 0);
                }        

                if (is_selected == x) {

                    // Executa update
                    if (rrn >= 0) {
                        update_vehicle(&V, y, update_fields, update_values);
                        long int file_offset = (rrn)*MAX_RRN + HEADER_SIZE_TYPE1;
                        fseek(file_bin_rw, file_offset, SEEK_SET);

                        write_reg_in_bin_type1(file_bin_rw, &V);
                    }
                }

                // Libera a memória alocada durante a leitura
                free_vehicle(&V);
                V = initialize_vehicle(1);
                rrn++;
            }

        }

        // Arquivo tipo 2
        else if (f_type == 2){

            long int offset = HEADER_SIZE_TYPE2;

            // Enquanto ainda houverem registros a serem lidos no arquivo de dados
            while(!read_reg_from_bin_type2(file_bin_rw, &V, &offset)){
                // Checa se atende à todas as condições do select
                is_selected = 0;
                for (int i=0; i<x; i++) {
                    if (strcmp("id", search_fields[i])!=0) 
                        is_selected = is_selected + check_meets_condition(V, search_fields[i], search_values[i], 0);
                }        
                if (is_selected == x) {

                    long int up_offset = offset - V.tamanhoRegistro - 5;

                    // Executa update
                    update_vehicle(&V, y, update_fields, update_values);
                    update_reg_type2(file_bin_rw, V, header, &up_offset, I_list, (*n_indices));
                }

                // Libera a memória alocada durante a leitura
                free_vehicle(&V);
                V = initialize_vehicle(2);
            }

        }
    }
    return 0;
}
