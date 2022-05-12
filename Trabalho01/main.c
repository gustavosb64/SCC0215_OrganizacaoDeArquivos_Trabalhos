#include <stdio.h>
#include <stdlib.h>
#include "./functions.h"

int main(int argc, char *argv[]){

    FILE *File = fopen("teste.csv","wb");

    write_header_type1(File);

    fclose(File);
    return 0;

}
