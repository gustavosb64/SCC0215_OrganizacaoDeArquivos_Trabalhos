#include <stdio.h>
#include <stdlib.h>
#include "./functions.h"

int main(int argc, char *argv[]){

    char *filename = readline(stdin);
    write_header_type1(filename);

    read_header_type1(filename);

//    fclose(File);
    return 0;

}
