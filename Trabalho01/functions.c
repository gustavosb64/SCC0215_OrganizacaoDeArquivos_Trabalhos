#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./functions.h"

#define MAX_RRN 97
#define HEADER_SIZE_TYPE1 182
#define HEADER_SIZE_TYPE2 189
struct header{
    char status;        // consistência do arquivo
    int tamanhoRegistro;    // tamanho do registro (usado apenas no tipo 2)
    union{
        int rrn;            // RRN do último registro logicamente removido (tipo 1)
        long int offset;    // offset do último registro logicamente removido (tipo 2)
    }topo;           
    char descricao[40]; // descrição dos metadados
    char desC1[22];     // descrição detalhada do campo 1
    char desC2[19];     // descrição detalhada do campo 2
    char desC3[24];     // descrição detalhada do campo 3
    char desC4[8];      // descrição detalhada do campo 4
    char codC5;         // descrição simplificada do campo 5
    char desC5[16];     // descrição detalhada do campo 5
    char codC6;         // descrição simplificada do campo 6
    char desC6[18];     // descrição detalhada do campo 6
    char codC7;         // descrição simplificada do campo 7
    char desC7[19];     // descrição detalhada do campo 7
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
char *readline(FILE *stream) {
    char *string = 0;
    int pos = 0; 

	do{
        if (pos % READLINE_BUFFER == 0) {
            string = (char *) realloc(string, (pos / READLINE_BUFFER + 1) * READLINE_BUFFER);
        }
        string[pos] = (char) fgetc(stream);
        pos++;
    }while(string[pos-1] != '\n' && !feof(stream));

    // Caso o último caractere seja \r, o próximo será um \n
    if (string[pos-1] == '\r')
        fgetc(stream);

    string[pos-1] = 0;
    string = (char *) realloc(string, pos);

    return string;
}

char *readfield(FILE *stream) {
    char *string = 0;
    int pos = 0; 

	do{
        if (pos % READLINE_BUFFER == 0) {
            string = (char *) realloc(string, (pos / READLINE_BUFFER + 1) * READLINE_BUFFER);
        }
        string[pos] = (char) fgetc(stream);
        pos++;
    }while(string[pos-1] != '\n' && string[pos-1] != ' ' && !feof(stream));

    // Caso o último caractere seja \r, o próximo será um \n
    if (string[pos-1] == '\r')
        fgetc(stream);

    string[pos-1] = 0;
    string = (char *) realloc(string, pos);

    return string;
}

void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

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


char *read_data(FILE *stream) {
    char *string = 0;
    int pos = 0; 

	do{
        if (pos % READLINE_BUFFER == 0) {
            string = (char *) realloc(string, (pos / READLINE_BUFFER + 1) * READLINE_BUFFER);
        }
        string[pos] = (char) fgetc(stream);
        pos++;
    }while(string[pos-1] != ',' && string[pos-1] != '\r' && string[pos-1] != '\n' && !feof(stream));

    // Caso o último caractere seja \r, o próximo será um \n
    if (string[pos-1] == '\r')
        fgetc(stream);

    string[pos-1] = 0;
    string = (char *) realloc(string, pos);

    return string;
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

int initialize_reg_type1(FILE *file_bin_w){

    // Preenche o espaço do registro com '$'
    char c = '$';
    for(int i=0; i<MAX_RRN; i++)
        fwrite(&c, sizeof(char), 1, file_bin_w);

    // Posiciona o cursor de leitura ao início do registro
    fseek(file_bin_w, -MAX_RRN, SEEK_CUR);
    
    return 0;
}

int write_header(FILE *file_header_w, int f_type){

    /* Motivos para não se ter utilizado a struct:
     *  Não se pode atribuir diretamente uma string a vetores de caracteres;
     *  porém, é possível declarar o vetor char já inicializado com a string desejada.
     *  Inicializando através da struct, não seria possível atribuir as strings diretamente aos seus componentes */
    char status = '1';

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

int write_reg_in_bin_type1(FILE *file_bin_w, Vehicle *V){

    // Inicializa o espaço dos próximos MAX_RRN bytes com o caractere '$'
    initialize_reg_type1(file_bin_w);

    // Conta quantos bytes devem ser pulados ao final devido ao padding
    int offset = 0;

    if ((*V).cidade != NULL){
        (*V).tamCidade = strlen((*V).cidade);
        (*V).codC5 = '0';
        offset += (sizeof(int) + sizeof(char) + (*V).tamCidade);
    }
    if ((*V).marca != NULL){
        (*V).tamMarca = strlen((*V).marca);
        (*V).codC6 = '1';
        offset += (sizeof(int) + sizeof(char) + (*V).tamMarca);
    }
    if ((*V).modelo != NULL){
        (*V).tamModelo = strlen((*V).modelo);
        (*V).codC7 = '2';
        offset += (sizeof(int) + sizeof(char) + (*V).tamModelo);
    }

    fwrite(&(*V).removido, sizeof(char), 1, file_bin_w);
    fwrite(&(*V).prox.rrn, sizeof(int), 1, file_bin_w);

    fwrite(&(*V).id, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).ano, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).qtt, sizeof(int), 1, file_bin_w);

    if ((*V).sigla == NULL){
        (*V).sigla = (char *) calloc (3, sizeof(char));
        strcpy((*V).sigla, "$$");
    }
    fwrite((*V).sigla, sizeof(char), 2, file_bin_w);

    offset += (sizeof(char) + 4*sizeof(int) + 2*sizeof(char));

    if ((*V).tamCidade){
        fwrite(&(*V).tamCidade, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC5, sizeof(char), 1, file_bin_w);
        fwrite((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_w);
    }

    if ((*V).tamMarca){
        fwrite(&(*V).tamMarca, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC6, sizeof(char), 1, file_bin_w);
        fwrite((*V).marca, sizeof(char), (*V).tamMarca, file_bin_w);
    }

    if ((*V).tamModelo){
        fwrite(&(*V).tamModelo, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC7, sizeof(char), 1, file_bin_w);
        fwrite((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_w);
    }

    // Posiciona o cursor do arquivo ao final do registro com padding
    offset = MAX_RRN - offset;
    fseek(file_bin_w, offset, SEEK_CUR);

    return 0;
}

int write_reg_in_bin_type2(FILE *file_bin_w, Vehicle *V, int *size_last_reg){

    if ((*V).cidade != NULL){
        (*V).tamCidade = strlen((*V).cidade);
        (*V).codC5 = '0';
    }
    if ((*V).marca != NULL){
        (*V).tamMarca = strlen((*V).marca);
        (*V).codC6 = '1';
    }
    if ((*V).modelo != NULL){
        (*V).tamModelo = strlen((*V).modelo);
        (*V).codC7 = '2';
    }

    fwrite(&(*V).removido, sizeof(char), 1, file_bin_w);

    // Pula campo de tamanhoRegistro e proxOffset para inserir depois
    fseek(file_bin_w, sizeof(int), SEEK_CUR); 

    // Armazena o byte offset onde os dados do registro começam
    long int start_byte = ftell(file_bin_w);

    fwrite(&(*V).prox.offset, sizeof(long int), 1, file_bin_w);

    fwrite(&(*V).id, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).ano, sizeof(int), 1, file_bin_w);
    fwrite(&(*V).qtt, sizeof(int), 1, file_bin_w);

    if ((*V).sigla == NULL){
        (*V).sigla = (char *) calloc (3, sizeof(char));
        strcpy((*V).sigla, "$$");
    }
    fwrite((*V).sigla, sizeof(char), 2, file_bin_w);

    if ((*V).tamCidade){
        fwrite(&(*V).tamCidade, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC5, sizeof(char), 1, file_bin_w);
        fwrite((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_w);
    }

    if ((*V).tamMarca){
        fwrite(&(*V).tamMarca, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC6, sizeof(char), 1, file_bin_w);
        fwrite((*V).marca, sizeof(char), (*V).tamMarca, file_bin_w);
    }

    if ((*V).tamModelo){
        fwrite(&(*V).tamModelo, sizeof(int), 1, file_bin_w);
        fwrite(&(*V).codC7, sizeof(char), 1, file_bin_w);
        fwrite((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_w);
    }

    // Armazena o último byte escrito e calcula o tamanho do registro
    long int end_byte = ftell(file_bin_w);
    (*V).tamanhoRegistro = end_byte - start_byte;

    // Retorna o ponteiro do arquivo nas posições reservadas ao tamanho e ao offset
    fseek(file_bin_w, start_byte - sizeof(int), SEEK_SET);

    fwrite(&(*V).tamanhoRegistro, sizeof(int), 1, file_bin_w);

    // Atualiza size_last_reg
    (*size_last_reg) = (*V).tamanhoRegistro;
    
    // Posiciona o ponteiro do arquivo ao final do registro escrito
    fseek(file_bin_w, 0, SEEK_END);

    return 0;
}

int read_reg_from_bin_type1(FILE *file_bin_r, Vehicle *V, int rrn){

    // Colocando o cursor de leitura no início do registro a ser buscado
    fseek(file_bin_r, MAX_RRN*rrn + HEADER_SIZE_TYPE1, SEEK_SET);

    // Contador de bytes utilizado para garantir que não se ultrapasse
    // o limite do registro
    int byte_counter = 0;

    // Usados na leitura para checagem dos dados
    char aux_char;

    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*V).removido, sizeof(char), 1, file_bin_r))
        return 1;

    fread(&(*V).prox.rrn, sizeof(int), 1, file_bin_r);
    fread(&(*V).id, sizeof(int), 1, file_bin_r);
    fread(&(*V).ano, sizeof(int), 1, file_bin_r);
    fread(&(*V).qtt, sizeof(int), 1, file_bin_r);

    (*V).sigla = (char *) malloc(2 * sizeof(char));
    fread((*V).sigla, sizeof(char), 2, file_bin_r);

    byte_counter += sizeof(int)*4 + sizeof(char)*3;

    for(int i=0; i<3; i++){

        // Caso a quantidade de bytes restantes ultrapasse MAX_RRN-5,
        // não há espaço para outro campo ter sido armazenado
        if (byte_counter > MAX_RRN-5) return 0;

        // Avança o cursor o tamanho de um inteiro para ler o caractere de 
        // descrição simplificada do campo
        // Caso fread() retorne 0, o arquivo atingiu o fim
        fseek(file_bin_r, 4, SEEK_CUR);
        if (!fread(&aux_char, sizeof(char), 1, file_bin_r)){
            // Retorna o cursor de leitura para o início do registro
            fseek(file_bin_r, -byte_counter-5, SEEK_CUR);
            return 0;
        }
        fseek(file_bin_r, -5, SEEK_CUR); // retorna o cursor para antes do inteiro

        // Caso o caractere lido não esteja entre 0 e 2, trata-se de lixo, 
        // e o registro terminou de ser lido
        if (aux_char < '0' || aux_char > '2'){

            // Retorna o cursor de leitura para o início do registro
            fseek(file_bin_r, -byte_counter, SEEK_CUR);
            return 0;
        }

        switch(aux_char){

            // Lê a cidade
            case '0':
                fread(&(*V).tamCidade, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC5, sizeof(char), 1, file_bin_r);
                (*V).cidade = (char *) malloc((*V).tamCidade * sizeof(char));

                fread((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_r);

                byte_counter += 1+4+(*V).tamCidade;
                break;

            // Lê a marca
            case '1':
                fread(&(*V).tamMarca, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC6, sizeof(char), 1, file_bin_r);
                (*V).marca = (char *) malloc((*V).tamMarca * sizeof(char));

                fread((*V).marca, sizeof(char), (*V).tamMarca, file_bin_r);

                byte_counter += 1+4+(*V).tamMarca;
                break;

            // Lê o modelo
            case '2':
                fread(&(*V).tamModelo, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC7, sizeof(char), 1, file_bin_r);
                (*V).modelo = (char *) malloc((*V).tamModelo * sizeof(char));

                fread((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_r);

                byte_counter += 1+4+(*V).tamModelo;
                break;

        }

    }

    // Retorna o cursor de leitura para o início do registro
    fseek(file_bin_r, -byte_counter, SEEK_CUR);

    return 0;

}

int read_reg_from_bin_type2(FILE *file_bin_r, Vehicle *V, long int *offset){

    // Colocando o cursor de leitura no início do registro a ser buscado
    fseek(file_bin_r, (*offset), SEEK_SET);

    // Contador de bytes utilizado para garantir que não se ultrapasse
    // o limite do registro
    int byte_counter = 0;

    // Usados na leitura para checagem dos dados
    char aux_char;

    // Caso não haja mais registros a serem lidos, retorna sinal de erro 1
    if (!fread(&(*V).removido, sizeof(char), 1, file_bin_r))
        return 1;

    fread(&(*V).tamanhoRegistro, sizeof(int), 1, file_bin_r);
    fread(&(*V).prox.offset, sizeof(long int), 1, file_bin_r);
    fread(&(*V).id, sizeof(int), 1, file_bin_r);
    fread(&(*V).ano, sizeof(int), 1, file_bin_r);
    fread(&(*V).qtt, sizeof(int), 1, file_bin_r);

    (*V).sigla = (char *) malloc(2 * sizeof(char));
    fread((*V).sigla, sizeof(char), 2, file_bin_r);

    byte_counter += sizeof(int)*4 + sizeof(long int) + sizeof(char)*3;

    for(int i=0; i<3; i++){

        // Caso a quantidade de bytes restantes ultrapasse o tamanhoRegistro,
        // não há espaço para outro campo ter sido armazenado
        if (byte_counter >= (*V).tamanhoRegistro){
            // Retorna o cursor de leitura para o início do registro
            (*offset) = ftell(file_bin_r);

            return 0;
        }

        // Avança o cursor o tamanho de um inteiro para ler o caractere de 
        // descrição simplificada do campo
        // Caso fread() retorne 0, o arquivo atingiu o fim
        fseek(file_bin_r, 4, SEEK_CUR);

        if (!fread(&aux_char, sizeof(char), 1, file_bin_r)){
            // Retorna o cursor de leitura para o início do registro
            (*offset) = ftell(file_bin_r);
            return 0;
        }
        fseek(file_bin_r, -5, SEEK_CUR); // retorna o cursor para antes do inteiro

        switch(aux_char){

            // Lê a cidade
            case '0':
                fread(&(*V).tamCidade, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC5, sizeof(char), 1, file_bin_r);
                (*V).cidade = (char *) malloc((*V).tamCidade * sizeof(char));

                fread((*V).cidade, sizeof(char), (*V).tamCidade, file_bin_r);

                byte_counter += sizeof(char) + sizeof(int) + (*V).tamCidade;
                break;

            // Lê a marca
            case '1':
                fread(&(*V).tamMarca, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC6, sizeof(char), 1, file_bin_r);
                (*V).marca = (char *) malloc((*V).tamMarca * sizeof(char));

                fread((*V).marca, sizeof(char), (*V).tamMarca, file_bin_r);

                byte_counter += sizeof(char) + sizeof(int) + (*V).tamMarca;
                break;

            // Lê o modelo
            case '2':
                fread(&(*V).tamModelo, sizeof(int), 1, file_bin_r);
                fread(&(*V).codC7, sizeof(char), 1, file_bin_r);
                (*V).modelo = (char *) malloc((*V).tamModelo * sizeof(char));

                fread((*V).modelo, sizeof(char), (*V).tamModelo, file_bin_r);

                byte_counter += sizeof(char) + sizeof(int) + (*V).tamModelo;
                break;

        }

    }

    // Retorna o cursor de leitura para o início do registro
    (*offset) = ftell(file_bin_r);

    return 0;

}

int read_all_reg_from_bin(char *filename_in_bin, int f_type){

    FILE *file_bin_r = fopen(filename_in_bin, "rb");
    if (file_bin_r == NULL){
        return 1;
    }

    Vehicle V = initialize_vehicle(f_type);

    // Realiza diferentes rotinas a depender do tipo a ser lido
    if (f_type == 1){

        char c_aux;
        fread(&c_aux, sizeof(char), 1, file_bin_r);
        if (c_aux == '0'){
            fclose(file_bin_r);
            return 1;
        }
        fseek(file_bin_r,0,SEEK_SET);

        int rrn = 0;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_reg_from_bin_type1(file_bin_r, &V, rrn)){

            // Imprime os dados do veículo
            print_vehicle(V,1);
            printf("\n");

            // Libera a memória alocada durante a leitura
            free_vehicle(&V);
            V = initialize_vehicle(1);
            rrn++;
        }

    }
    else if (f_type == 2){

        long int offset = HEADER_SIZE_TYPE2 + 1;

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

    // Caso nada seja lido no ID, não há mais dados no arquivo
    char *str_id = read_data(file_csv_r);
    if (str_id[0] == 0){
        free(str_id);
        return 1;
    }

    // Lendo corretamente os dados do arquivo csv
    int id = atoi(str_id);
    char *str_ano = read_data(file_csv_r);
    int ano = atoi(str_ano); 
    char *cidade = read_data(file_csv_r);
    char *str_qtt = read_data(file_csv_r);
    int qtt = atoi(str_qtt);
    char *sigla = read_data(file_csv_r);
    char *marca = read_data(file_csv_r); 
    char *modelo = read_data(file_csv_r);

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

    FILE *file_csv_r = fopen(filename_in_csv, "rb");
    FILE *file_bin_w = fopen(filename_out_bin, "wb");

    write_header(file_bin_w, f_type);
    
    // Lendo e liberando linha de cabeçalho
    free(readline(file_csv_r));

    Vehicle V = initialize_vehicle(f_type);

    // Realiza diferentes rotinas a depender do tipo do arquivo
    if (f_type == 1){

        // Contador de RRN, usado para marcar o próximo RRN no cabeçalho
        int rrn_counter = 1;

        // Enquanto ainda houverem dados a serem lidos
        while(!read_reg_from_csv(file_csv_r, &V)){

            // Escrevendo o registro 
            write_reg_in_bin_type1(file_bin_w, &V);
            free_vehicle(&V);

            // Preparando um novo registro a ser escrito
            V = initialize_vehicle(1);

            // Atualizando proxRRN no cabeçalho
            fseek(file_bin_w, 174, SEEK_SET);
            fwrite(&rrn_counter, sizeof(int), 1, file_bin_w);
            fseek(file_bin_w, 0, SEEK_END);

            rrn_counter++; //incrementando o RRN
        }

    }
    else if (f_type == 2){

        // Armazena o tamanho do último registro escrito, usado para setar como -1
        // o proxByteOffset do último registro do arquivo
        int size_last_reg = 0;
//        long int offset = HEADER_SIZE_TYPE2;
        long int cur_prox_offset = ftell(file_bin_w);
        while(!read_reg_from_csv(file_csv_r, &V)){

            // Escrevendo o registro 
            write_reg_in_bin_type2(file_bin_w, &V, &size_last_reg);

            // Atualizando proxByteOffset no cabeçalho
            cur_prox_offset = ftell(file_bin_w);
            fseek(file_bin_w, 178, SEEK_SET);
            fwrite(&cur_prox_offset, sizeof(long int), 1, file_bin_w);
            fseek(file_bin_w, 0, SEEK_END);

            free_vehicle(&V);

            // Preparando um novo registro a ser escrito
            V = initialize_vehicle(2);
        }
    }
    // Liberando memória
    fclose(file_bin_w);
    fclose(file_csv_r);

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

int search_vehicle_rrn(char *filename_in_bin ,int rrn) {
    FILE *file_bin_r = fopen(filename_in_bin, "rb");
    if (file_bin_r == NULL){
        return 1;
    }
    Vehicle V = initialize_vehicle(1);

    if (read_reg_from_bin_type1(file_bin_r, &V, rrn))
        return 2;

    // Imprime os dados do veículo
    print_vehicle(V,1);
    printf("\n");

    // Libera a memória alocada durante a leitura
    free_vehicle(&V);
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

    v_str[strlen(str)] = '\0';
    return strcmp(str, v_str);

}

int check_meets_condition(Vehicle V, char* field, char* value) {

    char* unquoted_value;
    if (strcmp(field, "id") == 0) {
        if(V.id == atoi(value)) return 1;
    } else if (strcmp(field, "marca") == 0) {
        unquoted_value = remove_quotes_str(value);
        if (V.marca != NULL && customized_strcmp(V.marca, unquoted_value) == 0){
            free(unquoted_value);
            return 1;
        }
        free(unquoted_value);
    } else if (strcmp(field, "cidade") == 0) {
        unquoted_value = remove_quotes_str(value);
        if (V.cidade != NULL && customized_strcmp(V.cidade, unquoted_value) == 0){
            free(unquoted_value);
            return 1;
        }
        free(unquoted_value);
    } else if (strcmp(field, "estado") == 0) {
        unquoted_value = remove_quotes_str(value);
        if (V.sigla != NULL && customized_strcmp(V.sigla, unquoted_value) == 0){
            free(unquoted_value);
            return 1;
        }
        free(unquoted_value);
    } else if (strcmp(field, "modelo") == 0) {
        unquoted_value = remove_quotes_str(value);
        if (V.modelo != NULL && customized_strcmp(V.modelo, unquoted_value) == 0) {
            free(unquoted_value);
            return 1;
        }
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
                is_selected = is_selected + check_meets_condition(V, fields[i], values[i]);
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

        long int offset = HEADER_SIZE_TYPE2 + 1;

        // Enquanto ainda houverem registros a serem lidos no arquivo de dados
        while(!read_reg_from_bin_type2(file_bin_r, &V, &offset)){

            // Checa se atende à todas as condições do select
            is_selected = 0;
            for (int i=0; i<n; i++) {
                is_selected = is_selected + check_meets_condition(V, fields[i], values[i]);
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
