#ifndef FUNCTIONS_H
#define FUNCTIONS_H


/*
 * Comment section
*/
typedef struct header Header;

/*
 * Comment section
*/
char *readline(FILE *stream) ;

/*
 * Comment section
*/
int write_header_type1(char *filename);

/*
 * Comment section
*/
int read_header_type1(char *filename);


#endif