/* Statement type, which forms the basis of all parsing. The top-level parsing
 * function should return an array of statements. At the top-level, any
 * non-expression statement is valid. Inside blocks, any non-function
 * statement is valid. The type of the statement is stored in statement_type,
 * while other possibly needed data is stored in the other fields.
 */
typedef struct _function {
} function;

typedef struct _block {
    struct _statement** statements;
    int num_statements;
} block;

typedef struct _statement {
    int statement_type;
    type* var_type;
    char* ident;
    expression* expr;
    function* func;
    block* code_block;
    struct _statement** children;
} statement;

/*** Statement types ***/

/* Assignment statement */
int STATEMENT_ASSIGN = 1;

/* Generic expression statement */
int STATEMENT_EXPRESSION = 2;

/* typedef */
int STATEMENT_TYPEDEF = 3;

/* Return */
int STATEMENT_RETURN = 4;

/* If/Else */
int STATEMENT_IF = 5;

/* Block */
int STATEMENT_BLOCK = 6;


/* Create a blank statement with a certain statement type */
statement* create_statement(int t){
    statement* st = calloc(1, sizeof(statement));
    st->statement_type = t;
    return st;
}

statement* parse_statement(token**, int*, int);

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
    if(st->statement_type == STATEMENT_RETURN){
        printf("RETURN ");
        print_expression(st->expr);
    }
    if(st->statement_type == STATEMENT_IF){
        printf("IF ");
        print_expression(st->expr);
        printf(" THEN ");
        print_statement(st->children[0]);
        if(st->children[1] != NULL){
            printf("ELSE ");
            print_statement(st->children[1]);
        }

    }
    printf("\n");
}

/* Release memory used by a statement */
void free_statement(statement* st){
    if(st->var_type != NULL)
        free_type(st->var_type);
    if(st->expr != NULL)
        free_expression(st->expr);
    if(st->ident != NULL)
        free(st->ident);
    if(st->children != NULL){
        int num_children = 0;
        if(st->statement_type == STATEMENT_IF)
            num_children = 2;

        for(int i = 0; i < num_children; i++)
            if(st->children[i] != NULL)
                free_statement(st->children[i]);
        free(st->children);
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

statement* create_return_statement(expression* val){
    statement* st = create_statement(STATEMENT_RETURN);
    st->expr = val;
    return st;
}

statement* create_if_statement(expression* cond, statement* then, statement* alternative){
    statement* if_st = create_statement(STATEMENT_IF);
    if_st->expr = cond;
    if_st->children = calloc(2, sizeof(statement*));
    if_st->children[0] = then;
    if_st->children[1] = alternative;
    return if_st;
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

statement* parse_block(token** tokens, int* index, int len){
    inc_ptr(index, len);
    queue* statement_list = make_queue();

    int count = 0;
    while(tokens[*index]->type != TOKEN_CBRACE){
        statement* next = parse_statement(tokens, index, len);
        enqueue(statement_list, next);
        count++;
    }

    statement** statements = calloc(count, sizeof(statement*));
    for(int i = 0; i < count; i++)
        statements[i] = dequeue(statement_list);

    free_queue(statement_list);

    block* b = calloc(1, sizeof(block));
    b->statements = statements;
    b->num_statements = count;

    statement* st = create_statement(STATEMENT_BLOCK);
    st->code_block = b;
    return st;
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
    if(first_token_type == TOKEN_RETURN){
        inc_ptr(index, len);
        expression* val = parse_expression(tokens, index, len, TOKEN_SEMICOLON);

        if(tokens[*index]->type != TOKEN_SEMICOLON)
            error("Expected semicolon after return statement");
        return create_return_statement(val);
    }
    if(first_token_type == TOKEN_IF){
        inc_ptr(index, len);
        if(tokens[*index]->type != TOKEN_OPAREN)
            error("Expected opening parenthesis after if statement");
        inc_ptr(index, len);

        expression* cond = parse_expression(tokens, index, len, TOKEN_CPAREN);

        if(tokens[*index]->type != TOKEN_CPAREN)
            error("Expected closing parenthesis after if statement condition");
        inc_ptr(index, len);

        statement* block = parse_statement(tokens, index, len);
        statement* alternative = NULL;
        inc_ptr(index, len);

        if(tokens[*index]->type == TOKEN_ELSE){
            inc_ptr(index, len);

            alternative = parse_statement(tokens, index, len);
        }

        return create_if_statement(cond, block, alternative);
    }
    if(first_token_type == TOKEN_OBRACE){
        statement* block = parse_block(tokens, index, len);
        return block;
    }

    /* Get type of declaration */
    type* type = parse_type(tokens, index, len);
    if(type == NULL){
        statement* st = create_statement(STATEMENT_EXPRESSION);
        st->expr = parse_expression(tokens, index, len, TOKEN_SEMICOLON);
        return st;
    }

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
        return create_assign_statement(type, ident, parse_expression(tokens, index, len, TOKEN_SEMICOLON));
    }
    if(next_token_type == TOKEN_OPAREN){
        inc_ptr(index, len);
        return create_function_statement(type, ident, parse_function(tokens, index, len));
    }
}
