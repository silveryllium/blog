/* Include C libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * This is the main file for the compiler. It contains the
 * main() function and calls the tokenizer, parser, and code
 * generator functions from the other files.
 */

/* Print help to the standard error */
void print_help(char** args){
    fprintf(stderr, "Usage: %s file1.c file2.c ...\n", args[0]); 
}

/* Main entry point: this is where the program starts */
int main(int argc, char** argv){
    /* Require at least one filename. If the user doesn't pass a filename,
     * print help and exit, because we have nothing to compile. */
    if(argc < 2){
        print_help(argv);
        exit(1);
    }

    /* Find total length of all the files */
    int total_length = 0;
    for(int i = 1; i < argc; i++)
        total_length += file_get_length(argv[i]);

    /* Allocate space for the text, initialize text to "" */
    char* text = calloc(1, total_length+1);
    text[0] = '\0';

    /* Read all files into our string */
    for(int i = 1; i < argc; i++){
        /* Get file contents, copy them over to our main text, and free them */
        char* file_contents = read_file(argv[i]);
        strcat(text, file_contents);
        free(file_contents);
    }

    /* Tokenize, and free source text memory */
    int num_tokens = 0;
    token** tokens = tokenize(text, &num_tokens);
    free(text);

    /* Parse tokens */
    init_parser();
    int index = 0;
    statement* statement = parse_statement(tokens, &index, num_tokens);
    print_statement(statement);

    /* Free all tokens */
    for(int i = 0; i < num_tokens; i++)
        free_token(tokens[i]);
    free(tokens);

    /* Return success */
    return 0;
}
