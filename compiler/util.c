#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Get the length of the file (number of characters) */
int file_get_length(char* fname){
    /* Open the file */
    FILE* file = fopen(fname, "r");

    /* Seek to the end */
    fseek(file, 0, SEEK_END);
    
    /* Get current position (since we're at the end, this is the file size) */
    long len = ftell(file);

    /* Close the file, return the length */
    fclose(file);
    return (int) len;
}

/* Read contents of text file */
char* read_file(char* fname){
    /* Get file length */
    long len = file_get_length(fname);

    /* Open file for reading */
    FILE* file = fopen(fname, "r");

    /* Allocate buffer for file data */
    char* buffer = (char*) calloc (1, sizeof(char)*len+1);

    /* Read data into buffer */
    int result = fread(buffer,1,len,file);

    /* Close file and return buffer */
    fclose (file);
    return buffer;
}

