/* Statement type, which forms the basis of all parsing. The top-level parsing
 * function should return an array of statements. At the top-level, any
 * non-expression statement is valid. Inside blocks, any non-function
 * statement is valid. The type of the statement is stored in statement_type,
 * while other possibly needed data is stored in the other fields.
 */
typedef struct _function {
} function;

typedef struct _statement {
    int statement_type;
    type* var_type;
    char* ident;
    expression* expr;
    function* func;
} statement;

/*** Statement types ***/

/* Assignment statement */
int STATEMENT_ASSIGN = 1;

/* Generic expression statement */
int STATEMENT_EXPRESSION = 2;

/* typedef */
int STATEMENT_TYPEDEF = 3;


/* Create a blank statement with a certain statement type */
statement* create_statement(int t){
    statement* st = calloc(1, sizeof(statement));
    st->statement_type = t;
    return st;
}

/* Print a statement in some useful debugging form to stdout */
void print_statement(statement* st){
    if(st->statement_type == STATEMENT_ASSIGN){
        printf("ASSIGN ");
        print_type(st->var_type);
        printf(" %s = ", st->ident);
        print_expression(st->expr);
    }
    else if(st->statement_type == STATEMENT_EXPRESSION){
        printf("EVAL ");
        print_expression(st->expr);
    }
    if(st->statement_type == STATEMENT_TYPEDEF){
        printf("TYPEDEF ");
        print_type(st->var_type);
        printf(" TO %s", st->ident);
    }
    printf("\n");
}

/* Release memory used by a statement */
void free_statement(statement* st){
    if(st->statement_type == STATEMENT_ASSIGN){
        free_type(st->var_type);
        free_expression(st->expr);
        free(st->ident);
    }

    free(st);
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

statement* create_typedef_statement(type* type, char* name){
    statement* st = create_statement(STATEMENT_TYPEDEF);
    st->var_type = type;
    st->ident = name;
    return st;
}

statement* create_function_statement(type* return_type, char* ident, function* func){
    return NULL;
}

function* parse_function(token** tokens, int* index, int len){
    return NULL;
}


statement* parse_typedef(token** tokens, int* index, int len){
    /* Skip typedef token */
    inc_ptr(index, len);

    type* aliased_type = parse_type(tokens, index, len);

    int first_token_type = tokens[*index]->type;
    if(first_token_type != TOKEN_IDENT)
        error("Expected identifier for typedef");
    
    char* alias = strdup(tokens[*index]->data);
    type* named_type = create_type(aliased_type->size);
    named_type->name = alias;
    named_type->alias = aliased_type;
    add_to_type_table(named_type);

    return create_typedef_statement(aliased_type, alias);
}

/* Parse any statement */
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

    inc_ptr(index, len);

    next_token_type = tokens[*index]->type;
    if(next_token_type == TOKEN_SEMICOLON){
        inc_ptr(index, len);
        return create_declaration_statement(type, ident);
    }
    if(next_token_type == TOKEN_ASSIGN){
        inc_ptr(index, len);
        return create_assign_statement(type, ident, parse_expression(tokens, index, len));
    }
    if(next_token_type == TOKEN_OPAREN){
        inc_ptr(index, len);
        return create_function_statement(type, ident, parse_function(tokens, index, len));
    }
}
