#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This file describes the tokenizer. The tokenizer takes a string
 * of source text and divides it into tokens, and returns an array
 * of token objects. Each token object has a type and associated
 * data, which is NULL if no associated data is necessary. 
 *
 * Each token represents a single piece of information from the 
 * C source file, whether it's a punctuation/syntax mark, a keyword,
 * an identifier, a string, etc etc.
 */

/* Token types */

/* Single character tokens: parentheses, braces, brackets, arithmetic, etc */

/* Parentheses: () */
int TOKEN_OPAREN = 1;
int TOKEN_CPAREN = 2;

/* Braces: {} */
int TOKEN_OBRACE = 3;
int TOKEN_CBRACE = 4;

/* Brackets: [] */
int TOKEN_OBRACKET = 5;
int TOKEN_CBRACKET = 6;

/* Arithmetic: + - * / */
int TOKEN_PLUS  = 7;
int TOKEN_MINUS = 8;
int TOKEN_TIMES = 9;
int TOKEN_DIV  = 10;
int TOKEN_ADDR = 11;

/* Punctuation: ; , .*/
int TOKEN_SEMICOLON = 12;
int TOKEN_DOT = 13;
int TOKEN_COMMA = 14;

/* Comparison and boolean operations: > < ! */
int TOKEN_GREATER = 15;
int TOKEN_LESS = 16;
int TOKEN_NOT = 17;

/* Assignment: = */
int TOKEN_ASSIGN = 18;

/* Keywords: typedef, int, char, other types, FILE, >=, ... */


/* Types: void, int, char, file, long */
int TOKEN_VOID = 19;
int TOKEN_INT = 20;
int TOKEN_CHAR = 21;
int TOKEN_FILE = 22;
int TOKEN_LONG = 23;

/* Control flow: if, while, for, return */
int TOKEN_RETURN = 24;
int TOKEN_IF = 25;
int TOKEN_WHILE = 26;
int TOKEN_FOR = 27;

/* Boolean operations: || && >= <= == */
int TOKEN_EQUALS = 28;
int TOKEN_OR = 29;
int TOKEN_AND = 30;
int TOKEN_GREATEREQ = 31;
int TOKEN_LESSEQ = 32;

/* Other keywords: typedef, struct, sizeof, -> */
int TOKEN_REF = 33;
int TOKEN_TYPEDEF = 34;
int TOKEN_STRUCT = 35;
int TOKEN_SIZEOF = 36;

/* Token types which have associated data: strings, characters, and identifiers */
int TOKEN_STRING = 37;
int TOKEN_CHARACTER = 38; 
int TOKEN_IDENT = 39;
int TOKEN_NUMBER = 40;

int TOKEN_INCR = 41;
int TOKEN_DECR = 42;

/* Used in the parser */
int TOKEN_UNARY_DEREF = 100;
int TOKEN_UNARY_MINUS = 200;
int TOKEN_POSTINCR = 300;
int TOKEN_POSTDECR = 400;
int TOKEN_PREINCR = 500;
int TOKEN_PREDECR = 600;

/* The token type, containing an int type and the associated data */
typedef struct _token {
    int type;
    char* data;
} token;

/* If the current index is on a single character token, create the token, else return NULL */
token* get_single_char_token(int index, char* text){
    /* All the types of single-character tokens and their corresponding characters */
    char single[18] = {'(', ')', '{', '}', '[', ']', '+', '-', '*', '/', ';', ',', '>', '<', '=', '.', '&', '!'};
    int single_token_types[18] = {TOKEN_OPAREN, TOKEN_CPAREN, TOKEN_OBRACE, TOKEN_CBRACE, TOKEN_OBRACKET, TOKEN_CBRACKET,
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_TIMES, TOKEN_DIV, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_GREATER, TOKEN_LESS, 
        TOKEN_ASSIGN, TOKEN_DOT, TOKEN_ADDR, TOKEN_NOT};
    int singles = 18;

    /* Test each type of token */
    for(int i = 0; i < singles; i++){
        /* If the token matches, create and return the token */
        if(text[index] == single[i]){
            token* t = calloc(1, sizeof(token));
            t->type = single_token_types[i];
            return t;
        }
    }

    /* If no token matches, return NULL for failure */
    return NULL;
}

/* If the current index is on a keyword, create a keyword token and return the length of it, else return NULL */
token* get_keyword_token(int index, char* text, int* len){
    /* All possible keywords */
    char* keywords[20] = {"typedef", "int", "char", "FILE", "long", "struct", "return",
             "equals", "||", "&&", ">=", "<=", "void", "sizeof", "for", "while", "if", "->", "++", "--"};
    int keyword_token_types[20] = {TOKEN_TYPEDEF, TOKEN_INT, TOKEN_CHAR, TOKEN_FILE, TOKEN_LONG, TOKEN_STRUCT, TOKEN_RETURN,
        TOKEN_EQUALS, TOKEN_OR, TOKEN_AND, TOKEN_GREATEREQ, TOKEN_LESSEQ, TOKEN_VOID, TOKEN_SIZEOF, 
        TOKEN_FOR, TOKEN_WHILE, TOKEN_IF, TOKEN_REF, TOKEN_INCR, TOKEN_DECR};
    int num_keywords = 20;

    /* Get length of text, so we don't overflow */
    int text_len = strlen(text);

    /* Check every keyword */
    for(int i = 0; i < num_keywords; i++){
        char* keyword = keywords[i];

        /* Test if this keyword matches */
        int found = 1;
        for(int j = 0; j < strlen(keyword); j++){
            if(index+j >= text_len || keyword[j] != text[index+j])
                found = 0;
        }

        /* If it matches, create and return the token */
        if(found){
            token* t = calloc(1, sizeof(token));
            t->type = keyword_token_types[i];
            *len = strlen(keyword);
            return t;
        }
    }

    /* If none of the keywords match, return NULL */
    return NULL;
}

/* Read a string token, starting with the character after the first quote */
token* get_string_token(int index, char* text, int* str_length){
    /* Store the start and end index */
    int start_index = index;
    int end_index = index;

    /* Store length of input text and length of string, so we don't overflow */
    int len = strlen(text);
    *str_length = 0;
    int num_chars = 0;

    /* Keep reading until the end */
    while(end_index < len && text[end_index] != '"'){
        /* If we see a backslash, ignore the next character */
        if(text[end_index] == '\\'){
            end_index+= 2;
            (*str_length)++;
        }
        else
            end_index++;

        /* Update the length of the string */
        (*str_length)++;
        num_chars ++;
    }

    /* Allocate space for the string, make it end with a \0 */
    char* data = calloc(1, num_chars + 1);
    data[*str_length] = '\0';

    /* Copy over the data into the string */
    for(int i = start_index; i < end_index; i++){
        char next_char = text[i];

        /* Ignore backslashes (treat the next character literally) */
        if(next_char == '\\'){
            next_char = text[++i];

            /* If it's an escape sequence, treat it as such */
            if(next_char == 'n')
                next_char = '\n';
            if(next_char == 't')
                next_char = '\t';
        }
        data[i - start_index] = next_char;
    }

    /* Create and return the token */
    token* tok = calloc(1, sizeof(token));
    tok->type = TOKEN_STRING;
    tok->data = data;
    return tok;
}

/* Read an identifier token, starting with the first character */
token* get_ident_token(int index, char* text, int* ident_length){
    /* Keep track of start and end index */
    int start_index = index;
    int end_index = index;
    
    /* Use length of input text to make sure we don't read too far */
    int len = strlen(text);

    /* Keep incrementing end index and length of identifier until we encounter End-of-File or a non-ident character */
    *ident_length = 0;
    while(end_index < len && 
            (text[end_index] == '_' || (text[end_index] >=  '0' && text[end_index] <=  '9') ||
             (text[end_index] >=  'a' && text[end_index] <=  'z') || (text[end_index] >=  'A' && text[end_index] <=  'Z'))){
        (*ident_length)++;
        end_index++;
    }

    /* Allocate space for the identifier, make the last character a terminator */
    char* data = calloc(1, *ident_length + 1);
    data[*ident_length] = '\0';

    /* Copy over data from input text to identifier string */
    for(int i = start_index; i < end_index; i++){
        char next_char = text[i];
        data[i - start_index] = next_char;
    }

    /* Create and return token */
    token* tok = calloc(1, sizeof(token));
    tok->type = TOKEN_IDENT;
    tok->data = data;
    return tok;
}

/* Read a number token, starting with the first character */
token* get_num_token(int index, char* text, int* num_length){
    /* Keep track of start and end index */
    int start_index = index;
    int end_index = index;
    
    /* Use length of input text to make sure we don't read too far */
    int len = strlen(text);

    /* Keep incrementing end index and length of identifier until we encounter End-of-File or a non-ident character */
    *num_length = 0;
    while(end_index < len && (text[end_index] >=  '0' && text[end_index] <=  '9')){
        (*num_length)++;
        end_index++;
    }

    /* Allocate space for the identifier, make the last character a terminator */
    char* data = calloc(1, *num_length + 1);
    data[*num_length] = '\0';

    /* Copy over data from input text to identifier string */
    for(int i = start_index; i < end_index; i++){
        char next_char = text[i];
        data[i - start_index] = next_char;
    }

    /* Create and return token */
    token* tok = calloc(1, sizeof(token));
    tok->type = TOKEN_NUMBER;
    tok->data = data;
    return tok;
}

/* Add the token to the list, allocating more space if necessary */
void add_token(token* tok, token*** tokens, int* allocated, int* num){
    /* If we don't have enough memory, allocate more */
    if(num > allocated){
        *allocated += 1000;
        *tokens = realloc(*tokens, *allocated);
    }

    /* Store the token, increment number of tokens we have */
    (*tokens)[*num] = tok;
    (*num)++;
}

/* Print a token for debugging */
void print_token(token* token){
    if(token->data == NULL)
        printf("%d\n", token->type);
    else
        printf("%d: %s\n", token->type, token->data);
}

/* Free a token and associated data */
void free_token(token* token){
    if(token->data != NULL)
        free(token->data);
    free(token);
}

/* Tokenize */
token** tokenize(char* text, int* num_tokens){
    /* Store length of text, number of allocated tokens, and number of actual tokens */
    int len = strlen(text);
    int allocated = 1000;
    int num = 0;

    /* Store where we are in the input text */
    int index = 0;

    /* Allocate space for the tokens */
    token** tokens = calloc(allocated, sizeof(token*));

    /* Until we reach EOF, keep adding tokens */
    while(index < len){
        token* tok = NULL;

        /* Comments: ignore everything until the ending * / */
        if(index + 1 < len && text[index] == '/' && text[index+1] == '*'){
            /* Account for starting / * */
            index += 2;

            /* Loop until end of comment */
            while(!(text[index] == '*' && text[index+1] == '/')){
                index++;
            }

            /* Account for ending * / */
            index += 2;
            continue;
        }

        /* Preprocessor macros: ignore everything after the # */
        if(text[index] == '#'){
            /* Account for # */
            index++;

            /* Forward until end of line */
            while(index < len && text[index] != '\n')
                index++;

            /* Account for newline */
            index++;
            continue;
        }

        /* Keyword tokens */
        int keyword_length = 0;
        if((tok = get_keyword_token(index, text, &keyword_length)) != NULL){
            add_token(tok, &tokens, &allocated, &num);

            /* Account for entire length of keyword */
            index += keyword_length;
            continue;
        }

        /* Single char tokens */
        if((tok = get_single_char_token(index, text)) != NULL){
            add_token(tok, &tokens, &allocated, &num);

            /* Only one character is used up */
            index++;
            continue;
        }

        /* String */
        if(text[index] == '"'){
            /* Account for the quote */
            index++;
            int str_length = 0;
            tok = get_string_token(index, text, &str_length);

            /* Account for length of string and ending quote */
            index += str_length + 1;
            add_token(tok, &tokens, &allocated, &num);
            continue;
        }

        /* Character surrounded by single quotes */
        if(text[index] == '\''){
            /* Account for quote */
            index++;

            /* Data string is just the character and terminating \0 */
            char* c = calloc(1, 2);
            c[1] = '\0';

            /* Escape sequences */
            if(text[index] == '\\' && text[index + 1] == '\'')
                c[0] = '\'';
            if(text[index] == '\\' && text[index + 1] == '\\')
                c[0] = '\\';
            if(text[index] == '\\' && text[index + 1] == '0')
                c[0] = '\0';
            if(text[index] == '\\' && text[index + 1] == 'n')
                c[0] = '\n';
            if(text[index] == '\\' && text[index + 1] == 't')
                c[0] = '\t';

            /* If it's not an escape sequence, just copy the character */
            else if(text[index] != '\\')
                c[0] = text[index];

            /* Account for possible backslash, the character itself, and ending quote */
            if(text[index] == '\\')
                index += 3;
            else
                index += 2;

            /* Allocate token and store it */
            tok = calloc(1, sizeof(token));
            tok->type = TOKEN_CHARACTER;
            tok->data = c;
            add_token(tok, &tokens, &allocated, &num);
            continue;
        }

        /* Ident: start with a-zA-Z_ and contains those and numbers */
        if(text[index] == '_' || (text[index] >=  'a' && text[index] <=  'z') || (text[index] >=  'A' && text[index] <=  'Z')){
            int ident_length = 0;
            tok = get_ident_token(index, text, &ident_length);
            add_token(tok, &tokens, &allocated, &num);

            /* Account for full length of token */
            index += ident_length;
            continue;
        }

        /* Number */
        if(text[index] >= '0' && text[index] <= '9'){
            int num_length = 0;
            tok = get_num_token(index, text, &num_length);
            add_token(tok, &tokens, &allocated, &num);

            /* Account for all digits */
            index += num_length;
            continue;
        }

        /* If we don't match any of the above conditions, it's probably whitespace, so ignore it */
        index++;
    }

    /* Store number of tokens found in num_tokens */
    *num_tokens = num;
    return tokens;
}

