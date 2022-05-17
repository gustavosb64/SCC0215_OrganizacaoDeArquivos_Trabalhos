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
    char *sigla;      // sigla do estado no qual o veículo está cadastrado
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
    FILE *File = fopen(filename, "wb");

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
    fwrite(&status, sizeof(char), 1, File);
    fwrite(&topo, sizeof(int), 1, File);
    fwrite(&descricao, sizeof(char), 40, File);
    fwrite(&desC1, sizeof(char), 22, File);
    fwrite(&desC2, sizeof(char), 19, File);
    fwrite(&desC3, sizeof(char), 24, File);
    fwrite(&desC4, sizeof(char), 8, File);
    fwrite(&codC5, sizeof(char), 1, File);
    fwrite(&desC5, sizeof(char), 16, File);
    fwrite(&codC6, sizeof(char), 1, File);
    fwrite(&desC6, sizeof(char), 18, File);
    fwrite(&codC7, sizeof(char), 1, File);
    fwrite(&desC7, sizeof(char), 19, File);
    fwrite(&proxRNN, sizeof(int), 1, File);
    fwrite(&nroRegRem, sizeof(int), 1, File);

    fclose(File);

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
    FILE *File = fopen(filename, "rb");
    if (File == NULL){
        printf("ERRO! Arquivo não encontrado.");
        return 1;
    }

    // Lê os dados do arquivo e os armazenando em um Header
    Header H; 

    fseek(File, 0, SEEK_SET);
    fread(&H.status, sizeof(char), 1, File);
    fread(&H.topo, sizeof(int), 1, File);
    fread(&H.descricao, sizeof(char), 40, File);
    fread(&H.desC1, sizeof(char), 22, File);
    fread(&H.desC2, sizeof(char), 19, File);
    fread(&H.desC3, sizeof(char), 24, File);
    fread(&H.desC4, sizeof(char), 8, File);
    fread(&H.codC5, sizeof(char), 1, File);
    fread(&H.desC5, sizeof(char), 16, File);
    fread(&H.codC6, sizeof(char), 1, File);
    fread(&H.desC6, sizeof(char), 18, File);
    fread(&H.codC7, sizeof(char), 1, File);
    fread(&H.desC7, sizeof(char), 19, File);
    fread(&H.proxRNN, sizeof(int), 1, File);
    fread(&H.nroRegRem, sizeof(int), 1, File);

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

    fclose(File);
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
    V.tamCidade = -1;
    V.codC5 = -1;
    V.cidade = NULL;
    V.tamMarca = -1;
    V.codC6 = -1;
    V.marca = NULL;
    V.tamModelo = -1;
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
int read_csv_type1(char *filename){

    FILE *File = fopen(filename, "rb");
    
    char *string = NULL;
    char *aux_str = NULL;
    free(readline(File));

    Vehicle V = initialize_vehicle();

    char *str_id;
    int id;
    char *str_ano;
    int ano; 
    char *cidade;
    char *str_qtt;
    int qtt;
    char *sigla;
    char *marca; 
    char *modelo;

//    for(int i=0; i<5; i++){
    do{

        str_id = read_data(File);
        id = atoi(str_id);
        str_ano = read_data(File);
        ano = atoi(str_ano);
        cidade = read_data(File);
        str_qtt = read_data(File);
        qtt = atoi(str_qtt);
        sigla = read_data(File);
        marca = read_data(File);
        modelo = read_data(File);

        if (id) printf("id: %d\n", id);
        if (ano) printf("ano: %d\n", ano);
        if (cidade[0]) printf("cidade: %s\n", cidade);
        if (qtt) printf("qtt: %d\n", qtt);
        if (sigla[0]) printf("marca: %s\n", sigla);
        if (marca[0]) printf("marca: %s\n", marca);
        if (modelo[0]) printf("modelo: %s\n", modelo);

        printf("---------\n");

        free(str_id);
        free(str_ano);
        free(str_qtt);
        free(cidade);
        free(sigla);
        free(marca);
        free(modelo);

    }while(str_id[0]);

    fclose(File);

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
