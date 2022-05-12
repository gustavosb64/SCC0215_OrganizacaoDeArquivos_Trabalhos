#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./functions.h"

#define MAX_RRN 97
struct header{
    char status;        // consistência do arquivo
    int topo;           // RRN do último registro logicamente removido
    char descricao[40]; // descrição dos metadados
    char desC1[20];     // descrição detalhada do campo 1
    char desC2[19];     // descrição detalhada do campo 2
    char desC3[24];     // descrição detalhada do campo 3
    char desC4[8];      // descrição detalhada do campo 4
    char codC5;         // descrição simplificada do campo 5
    char desC5[17];     // descrição detalhada do campo 5
    char codC6;         // descrição simplificada do campo 6
    char desC6[18];     // descrição detalhada do campo 6
    char codC7;         // descrição simplificada do campo 7
    char desC7[19];     // descrição detalhada do campo 7
    int proxRNN;        // próximo RRN disponível
    int nroRegRem;      // quantidade de registros logicamente removidos
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
    }while(string[pos++] != '\n' && string[pos-1] != '\r' && !feof(stream));

    string[pos-1] = 0;
    string = (char *) realloc(string, pos);

    return string;
}

int write_header_type1(char *filename){

    // Abre arquivo para escrita 
    FILE *File = fopen(filename, "wb");

    char status = 0;
    int topo = -1;
    char descricao[41] = "LISTAGEM DA FROTA DOS VEICULOS NO BRASIL";
    char desC1[21] = "CODIGO IDENTIFICADOR:";
    char desC2[20] = "ANO DE FABRICACAO:";
    char desC3[25] = "QUANTIDADE DE VEICULOS:";
    char desC4[9] = "ENERGIA:"; 
    char codC5 = 0;    
    char desC5[17] = "NOME DA CIDADE:";
    char codC6 = 1;    
    char desC6[18] = "MARCA DO VEICULO:";
    char codC7 = 2;    
    char desC7[19] = "MODELO DO VEICULO:";
    int proxRNN = -1;        
    int nroRegRem = 0;      

    fwrite(&status, sizeof(char), 1, File);
    fwrite(&topo, sizeof(int), 1, File);
    fwrite(&descricao, sizeof(char), 40, File);
    fwrite(&desC1, sizeof(char), 20, File);
    fwrite(&desC2, sizeof(char), 19, File);
    fwrite(&desC3, sizeof(char), 24, File);
    fwrite(&desC4, sizeof(char), 8, File);
    fwrite(&codC5, sizeof(char), 1, File);
    fwrite(&desC5, sizeof(char), 17, File);
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

    Header H; 

    fseek(File, 0, SEEK_SET);
    fread(&H.status, sizeof(char), 1, File);
    fread(&H.topo, sizeof(int), 1, File);
    fread(&H.descricao, sizeof(char), 40, File);
    fread(&H.desC1, sizeof(char), 20, File);
    fread(&H.desC2, sizeof(char), 19, File);
    fread(&H.desC3, sizeof(char), 24, File);
    fread(&H.desC4, sizeof(char), 8, File);
    fread(&H.codC5, sizeof(char), 1, File);
    fread(&H.desC5, sizeof(char), 17, File);
    fread(&H.codC6, sizeof(char), 1, File);
    fread(&H.desC6, sizeof(char), 18, File);
    fread(&H.codC7, sizeof(char), 1, File);
    fread(&H.desC7, sizeof(char), 19, File);
    fread(&H.proxRNN, sizeof(int), 1, File);
    fread(&H.nroRegRem, sizeof(int), 1, File);

    printf("%d\n",H.status);
    printf("%d\n",H.topo);
    print_string(H.descricao, 40);
    print_string(H.desC1, 20);
    print_string(H.desC2, 19);
    print_string(H.desC3, 24);
    print_string(H.desC4, 8);
    printf("%d\n",H.codC5);
    print_string(H.desC5, 17);
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
