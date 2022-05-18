#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./functions.h"

#define MAX_RRN 97
struct header{
    char status;        // consistência do arquivo
    int topo;           // RRN do último registro logicamente removido
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
    int proxRNN;        // próximo RRN disponível
    /*
     * union{
     *  int proxRNN;
     *  long int proxByteOffset;
     * }prox;
     *
     */
    int nroRegRem;      // quantidade de registros logicamente removidos
};

struct vehicle{
    char removido;      // indica se o registro está logicamente removido
    int prox;           // armazena o RRN do próximo registro
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

    if (string[pos-2] == 'r'){
        string[pos-2] = 0;
        string = (char *) realloc(string, pos-1);
    }
    else{
        string[pos-1] = 0;
        string = (char *) realloc(string, pos);
    }

    return string;
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

int write_header_type1(char *filename){

    // Abre arquivo para escrita 
    FILE *file_header_w = fopen(filename, "wb");

    /* Motivos para não se ter utilizado a struct:
     *  Não se pode atribuir diretamente uma string a vetores de caracteres;
     *  porém, é possível declarar o vetor char já inicializado com a string desejada.
     *  Inicializando através da struct, não seria possível atribuir as strings diretamente aos seus componentes */
    char status = 0;
    int topo = -1;
    char descricao[41] = "LISTAGEM DA FROTA DOS VEICULOS NO BRASIL";
    char desC1[22] = "CODIGO IDENTIFICADOR: ";
    char desC2[19] = "ANO DE FABRICACAO: ";
    char desC3[24] = "QUANTIDADE DE VEICULOS: ";
    char desC4[8] = "ESTADO: "; 
    char codC5 = 0;    
    char desC5[16] = "NOME DA CIDADE: ";
    char codC6 = 1;    
    char desC6[18] = "MARCA DO VEICULO: ";
    char codC7 = 2;    
    char desC7[19] = "MODELO DO VEICULO: ";
    int proxRNN = -1;        
    int nroRegRem = 0;      

    // Escreve os dados no arquivo
    fwrite(&status, sizeof(char), 1, file_header_w);
    fwrite(&topo, sizeof(int), 1, file_header_w);
    fwrite(&descricao, sizeof(char), 40, file_header_w);
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
    fwrite(&proxRNN, sizeof(int), 1, file_header_w);
    fwrite(&nroRegRem, sizeof(int), 1, file_header_w);

    fclose(file_header_w);

    return 0;
}

int print_string(char string[], int len){

    // Conferindo se comprimento fornecido é maior que 0
    if (len <= 0){
        printf("ERRO! Comprimento inválido.");
        return 1;
    }

    // Imprimindo na saída padrão os caracteres da string
    for (int i=0; i<len; i++)
        printf("%c",string[i]);

    printf("\n");

    return 0;
}

int read_header_type1(char *filename){

    // Abre arquivo para leitura e checa se ele existe
    FILE *file_header_r = fopen(filename, "rb");
    if (file_header_r == NULL){
        printf("ERRO! Arquivo não encontrado.");
        return 1;
    }

    // Lê os dados do arquivo e os armazenando em um Header
    Header H; 

    fseek(file_header_r, 0, SEEK_SET);
    fread(&H.status, sizeof(char), 1, file_header_r);
    fread(&H.topo, sizeof(int), 1, file_header_r);
    fread(&H.descricao, sizeof(char), 40, file_header_r);
    fread(&H.desC1, sizeof(char), 22, file_header_r);
    fread(&H.desC2, sizeof(char), 19, file_header_r);
    fread(&H.desC3, sizeof(char), 24, file_header_r);
    fread(&H.desC4, sizeof(char), 8, file_header_r);
    fread(&H.codC5, sizeof(char), 1, file_header_r);
    fread(&H.desC5, sizeof(char), 16, file_header_r);
    fread(&H.codC6, sizeof(char), 1, file_header_r);
    fread(&H.desC6, sizeof(char), 18, file_header_r);
    fread(&H.codC7, sizeof(char), 1, file_header_r);
    fread(&H.desC7, sizeof(char), 19, file_header_r);
    fread(&H.proxRNN, sizeof(int), 1, file_header_r);
    fread(&H.nroRegRem, sizeof(int), 1, file_header_r);

    // Imprime os dados lidos
    printf("%d\n",H.status);
    printf("%d\n",H.topo);
    print_string(H.descricao, 40);
    print_string(H.desC1, 22);
    print_string(H.desC2, 19);
    print_string(H.desC3, 24);
    print_string(H.desC4, 8);
    printf("%d\n",H.codC5);
    print_string(H.desC5, 16);
    printf("%d\n",H.codC6);
    print_string(H.desC6, 18);
    printf("%d\n",H.codC7);
    print_string(H.desC7, 19);
    printf("%d\n",H.proxRNN);
    printf("%d\n",H.nroRegRem);

    fclose(file_header_r);
    free(filename);

    return 0;

}
/*
struct vehicle{
    char removido;      // indica se o registro está logicamente removido
    int prox;           // armazena o RRN do próximo registro
    int id;             // código identificador
    int ano;            // ano de fabricação
    int qtt;            // quantidade de veículos
    char sigla[2];      // sigla do estado no qual o veículo está cadastrado
    int tamCidade;      // tamanho do campo cidade
    char codC5;         // descrição simplificada do campo 5
    char *cidade;       // nome da cidade
    int tamMarca;       // tamanho do campo marca
    char codC6;         // descrição simplificada do campo 5
    char *marca;        // nome da marca
    int tamModelo;      // tamanho do campo modelo
    char codC7;         // descrição simplificada do campo 5
    char *modelo;       // nome do modelo
}
*/

// Inicializa dado do tipo Veículo com os valores nulos padrão
Vehicle initialize_vehicle(){

    Vehicle V;

    V.removido = 0;
    V.prox = -1;
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

// AUTOTAD_PRIVATE
int read_one_reg_csv(FILE *file){

    if (file == NULL)
        return 1;

//    Vehicle V = initialize_vehicle();
    

    return 0;

}

// id,anoFabricacao,cidade,quantidade,siglaEstado,marca,modelo
int read_file_type1(FILE *file_csv_r, Vehicle *V){
    
    if (fread(&(*V).id, sizeof(char), 1, file_csv_r))
        return 1;

    fread(&(*V).ano, sizeof(char), 1, file_csv_r);

    return 0;

}

int write_file_type1(FILE *file_csv_w, Vehicle *V){

    if (file_csv_w == NULL){
        return 1;
    }

    if ((*V).cidade != NULL){
        (*V).tamCidade = strlen((*V).cidade);
        (*V).codC5 = 0;
    }
    if ((*V).marca != NULL){
        (*V).tamMarca = strlen((*V).marca);
        (*V).codC6 = 1;
    }
    if ((*V).modelo != NULL){
        (*V).tamModelo = strlen((*V).modelo);
        (*V).codC7 = 2;
    }

    fwrite(&(*V).removido, sizeof(char), 1, file_csv_w);
    fwrite(&(*V).prox, sizeof(int), 1, file_csv_w);
    fwrite(&(*V).id, sizeof(int), 1, file_csv_w);
    fwrite(&(*V).ano, sizeof(int), 1, file_csv_w);

    if ((*V).tamCidade){
        fwrite(&(*V).tamCidade, sizeof(int), 1, file_csv_w);
        fwrite(&(*V).codC5, sizeof(char), 1, file_csv_w);
        fwrite((*V).cidade, sizeof(char), (*V).tamCidade, file_csv_w);
    }

    fwrite(&(*V).qtt, sizeof(int), 1, file_csv_w);

    if ((*V).sigla == NULL){
        (*V).sigla = (char *) malloc (3 * sizeof(char));
        strcpy((*V).sigla, "&&");
    }
    fwrite(&(*V).sigla, sizeof(char), 2, file_csv_w);

    if ((*V).tamMarca){
        fwrite(&(*V).tamMarca, sizeof(int), 1, file_csv_w);
        fwrite(&(*V).codC6, sizeof(char), 1, file_csv_w);
        fwrite((*V).marca, sizeof(char), (*V).tamMarca, file_csv_w);
    }

    if ((*V).tamModelo){
        fwrite(&(*V).tamModelo, sizeof(int), 1, file_csv_w);
        fwrite(&(*V).codC7, sizeof(char), 1, file_csv_w);
        fwrite((*V).modelo, sizeof(char), (*V).tamModelo, file_csv_w);
    }

    return 0;
}

int read_file_type1(FILE *file_csv_r, Vehicle *V){

    // Caso o nada seja lido no ID, não há mais dados no arquivo
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

    /*
    if (id) printf("id: %d\n", id);
    if (ano) printf("ano: %d\n", ano);
    if (cidade[0]) printf("cidade: %s\n", cidade);
    if (qtt) printf("qtt: %d\n", qtt);
    if (sigla[0]) printf("marca: %s\n", sigla);
    if (marca[0]) printf("marca: %s\n", marca);
    if (modelo[0]) printf("modelo: %s\n", modelo);

    printf("---------\n");
    */

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

    // Liberando memória
    free(str_id);
    free(str_ano);
    free(str_qtt);

    return 0;
}

int read_csv_type1(char *filename){

    // Abrindo arquivo csv para leitura
    FILE *file_csv_r = fopen(filename, "rb");

    char *filename_w = (char *) calloc( 12, sizeof(char));
    strcpy(filename_w, "frota_w.bin");
    FILE *file_bin_w = fopen(filename_w, "wb");
    
    // Lendo e liberando linha de cabeçalho
    free(readline(file_csv_r));

    // Enquanto ainda houverem dados a serem lidos
    Vehicle V = initialize_vehicle();
    while(!read_file_type1(file_csv_r, &V)){
//        print_vehicle(V);
        
        write_file_type1(file_bin_w, &V);

        free_vehicle(&V);
        V = initialize_vehicle();
//        printf("-----------\n");
    }

    free(filename_w);
    fclose(file_bin_w);
    fclose(file_csv_r);

    return 0;

}

int print_vehicle(Vehicle V){

    printf("Removido: %d\n", V.removido);
    printf("Próximo RRN: %d\n", V.prox);
    printf("ID: %d\n", V.id);             
    printf("Ano de fabricação: %d\n", V.ano);            
    printf("tamCidade: %d\n", V.tamCidade);      
    printf("codC5: %d\n", V.codC5);         
    printf("Nome da cidade: %s\n", V.cidade);       
    printf("Quantidade de carros: %d\n", V.qtt);            
    printf("Estado: %s\n", V.sigla);      
    printf("tamMarca: %d\n", V.tamMarca);       
    printf("codC6: %d\n", V.codC6);         
    printf("Nome da marca: %s\n", V.marca);        
    printf("tamModelo: %d\n", V.tamModelo);      
    printf("codC7: %d\n", V.codC7);         
    printf("Nome do modelo: %s\n", V.modelo);       
    
    return 0;
}

int free_vehicle(Vehicle *V){

    free((*V).sigla );
    free((*V).cidade );
    free((*V).marca );
    free((*V).modelo );

    return 0;
}
