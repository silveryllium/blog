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
