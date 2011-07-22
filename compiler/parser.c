/* Include C libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This file describes the statement and expression parser. The parser
 * converts the token stream into a series of statements to be compiled
 * into machine code later. 
 *
 * Unlike some languages such as Lisp or Ruby, C differentiates between 
 * statements and expressions, so the parser has two separate data structures
 * for the two types of syntax elements. Note that since some expressions also
 * double as full-fledged statements (such as var++ and x = 5), there is also
 * a generic statement which stores an inner expression.
 *
 * This parser uses a modified Shunning-Yard algorithm for parsing infix 
 * expressions. It uses a look-behind token to resolve some semantic
 * conflicts, such as whether the asterisk * means multiplication or
 * dereferencing. (x + *y has a different meaning completely from x * y.)
 *
 * In addition to doing parsing, the parser creates and maintains a type table
 * which stores all the known primitive types and any user defined types. The
 * types stored in this type table can later be used for compiling to assembly.
 */

/* Initialize all aspects of the parser */
void init_parser(){
    init_type_table();
}

/* Delete all parser resources */
void del_parser(){
    del_type_table();
}

statement* create_declaration_statement(type* type, char* ident){
    statement* st = create_statement(STATEMENT_ASSIGN);
    st->var_type = type;
    st->ident = ident;
    st->expr = zero_expression();
    return st;
}

statement* create_assign_statement(type* type, char* ident, expression* expr){
    statement* st = create_statement(STATEMENT_ASSIGN);
    st->var_type = type;
    st->ident = ident;
    st->expr = expr;
    return st;
}



statement* parse_typedef(token** tokens, int* index, int len){
    return NULL;
}

statement* create_function_statement(type* return_type, char* ident, function* func){
    return NULL;
}

function* parse_function(token** tokens, int* index, int len){
    return NULL;
}

/* Parse a statement */
statement* parse_statement(token** tokens, int* index, int len){
    if(*index >= len) error("Unexpected end of token stream");

    int first_token_type = tokens[*index]->type;
    /* Typedef */
    if(first_token_type == TOKEN_TYPEDEF)
        return parse_typedef(tokens, index, len);

    /* Get type of declaration */
    type* type = parse_type(tokens, index, len);

    /* Get identifier for declaration */
    int next_token_type = tokens[*index]->type;
    if(next_token_type != TOKEN_IDENT)
        error("Expected identifier");
    char* ident = strdup(tokens[*index]->data);

    (*index)++;
    if(*index >= len) error("Unexpected end of token stream");

    next_token_type = tokens[*index]->type;
    if(next_token_type == TOKEN_SEMICOLON){
        (*index)++;
        return create_declaration_statement(type, ident);
    }
    if(next_token_type == TOKEN_ASSIGN){
        (*index)++;
        if(*index >= len) error("Unexpected end of token stream");
        return create_assign_statement(type, ident, parse_expression(tokens, index, len));
    }
    if(next_token_type == TOKEN_OPAREN){
        (*index)++;
        if(*index >= len) error("Unexpected end of token stream");
        return create_function_statement(type, ident, parse_function(tokens, index, len));
    }
}
