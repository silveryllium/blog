/* Include C libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _type {
    struct _type* ptr_to;
    int size;
    char* name;
} type;

typedef struct _expression {
    int expression_type;
    int num_children;
    struct _expression** children;
    int num_value;
    char* str_value;
} expression;

int EXPRESSION_NUM_CONST = 1;

typedef struct _function {

} function;

typedef struct _statement {
    int statement_type;
    type* var_type;
    char* ident;
    expression* expr;
    function* func;
} statement;

int STATEMENT_ASSIGN = 1;

struct {
    int num;
    type** types;
} type_table;

int TYPE_VOID = 0;
int TYPE_CHAR = 1;
int TYPE_INT = 2;

type* create_type(int size){
    type* t = calloc(1, sizeof(type));
    t->size = size;
    return t;
}

type* create_type_ptr(type* point_to){
    type* type = create_type(4);
    type->ptr_to = point_to;
    return type;
}

void init_type_table(){
    /* Initialize known primitive types */
    type_table.num = 3;
    type_table.types = calloc(type_table.num, sizeof(type*));

    type* type_void = create_type(0);
    type* type_int = create_type(4);
    type* type_char = create_type(1);

    type_void->name = "void";
    type_int->name = "int";
    type_char->name = "char";

    type_table.types[TYPE_VOID] = type_void;
    type_table.types[TYPE_INT] = type_int;
    type_table.types[TYPE_CHAR] = type_char;
}

void init_parser(){
    init_type_table();

}

expression* create_expression(int t){
    expression* expr = calloc(1, sizeof(expression));
    expr->expression_type = t;
    expr->num_children = 0;
    return expr;
}

expression* numeric_constant_expression(int num){
    expression* expr = create_expression(EXPRESSION_NUM_CONST);
    expr->num_value = num;
    return expr;
}

expression* zero_expression(){
    return numeric_constant_expression(0);
}

statement* create_statement(int t){
    statement* st = calloc(1, sizeof(statement));
    st->statement_type = t;
    return st;
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

void print_expression(expression* expr){
    if(expr->expression_type == EXPRESSION_NUM_CONST)
        printf("%d ", expr->num_value);
    else
        printf("type(%d) ", expr->expression_type);
}

void print_type(type* type){
    if(type->ptr_to == NULL)
        printf("%s", type->name);
    else {
        print_type(type->ptr_to);
        printf("*");
    }
}

void print_statement(statement* st){
    if(st->statement_type == STATEMENT_ASSIGN){
        printf("ASSIGN ");
        print_type(st->var_type);
        printf(" %s = ", st->ident);
        print_expression(st->expr);
    }

}

type* parse_type(token** tokens, int* index, int len){
    int first_type = tokens[*index]->type;
    (*index)++;
    if(first_type == TOKEN_INT)
        return type_table.types[TYPE_INT];
    if(first_type == TOKEN_CHAR)
        return type_table.types[TYPE_CHAR];
    if(first_type == TOKEN_VOID)
        return type_table.types[TYPE_VOID];

    else return NULL;
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

expression* parse_expression(token** tokens, int* index, int len){
    return NULL;
}

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
