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

int write_header_type1(FILE *File){

    char status = 0;
    char topo = -1;
    char descricao[41] = "LISTAGEM DA FROTA DOS VEICULOS NO BRASIL";
    char desC1[21] = "CODIGO IDENTIFICADOR:";
    char desC2[20] = "ANO DE FABRICACAO:";
    char desC3[25] = "QUANTIDADE DE VEICULOS";
    char desC4[9] = "ENERGIA"; 
    char codC5 = 0;    
    char desC5[17] = "NOME DA CIDADE";
    char codC6 = 1;    
    char desC6[18] = "MARCA DO VEICULO";
    char codC7 = 2;    
    char desC7[19] = "MODELO DO VEICULO";
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

    return 0;
}
