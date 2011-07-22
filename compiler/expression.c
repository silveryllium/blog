/* An expression type, which defines an expression that returns a value. The
 * type of the expression is stored in expression_type, while the rest of the
 * fields describe possible data needed for expression evaluation. Many 
 * expressions utilize sub-expressions, stored in the children array, while
 * constant expressions will utilize the num_value and char_value fields to
 * store the actual values of the constants. chars are just single-character
 * strings in this implementation.
 */
typedef struct _expression {
    int expression_type;
    int num_children;
    struct _expression** children;
    int num_value;
    char* str_value;
} expression;

/*** Expression types ***/

/* Constants (numeric, string, character literals) */
int EXPRESSION_NUM_CONST = 1;
int EXPRESSION_STR_CONST = 2;
int EXPRESSION_CHR_CONST = 3;

/* Arithmetic expressions, two children each */
int EXPRESSION_ARITH_ADD = 4;
int EXPRESSION_ARITH_SUB = 5;
int EXPRESSION_ARITH_MUL = 6;
int EXPRESSION_ARITH_DIV = 7;

/* Identifier to be evaluated. Name stored in str_value. */
int EXPRESSION_IDENT = 8;

/* Function call (variable number of children) */
int EXPRESSION_FUNCALL = 9;

/* 
 * Array access with two children, the first being the array
 * and the second being the index to to retrieve.
 */
int EXPRESSION_ARRAY_ACCESS = 10;

/* Dereferencing operator, single child */
int EXPRESSION_DEREF = 11;

/* Referencing operator, single child */
int EXPRESSION_ADDR = 12;

/* Increment and decrement expressions, single child */
int EXPRESSION_POSTINCR = 13;
int EXPRESSION_POSTDECR = 14;
int EXPRESSION_PREINCR = 53;
int EXPRESSION_PREDECR = 54;

/* 
 * Assignment expression with two children. The first child 
 * should be an identifier expression.
 */
int EXPRESSION_ASSIGN = 15;

/* Boolean operators with two children */
int EXPRESSION_AND = 16;
int EXPRESSION_OR = 17;
int EXPRESSION_EQUALS = 18;
int EXPRESSION_GREATER = 19;
int EXPRESSION_LESS = 20;
int EXPRESSION_GREATEREQ = 21;
int EXPRESSION_LESSEQ = 22;

/* Unary negation operator */
int EXPRESSION_NOT = 23;

/* Create a blank expression of a given type */
expression* create_expression(int t){
    expression* expr = calloc(1, sizeof(expression));
    expr->expression_type = t;
    expr->num_children = 0;
    return expr;
}

void print_expression(expression* expr);

/* Print a binary operator expression */
void print_binary_expression(char* operator, expression* expr){
    printf("(");
    print_expression(expr->children[0]);
    printf(" %s ", operator);
    print_expression(expr->children[1]);
    printf(")");

}

/* Prints a unary operator expression */
void print_unary_expression(char* operator, expression* expr){
    printf("%s", operator);
    print_expression(expr->children[0]);
}

/* Print an expression in some useful debugging form to stdout.
 * The print output will attempt to mirror the original source.
 */
void print_expression(expression* expr){
    /* Constants printed in literal form */
    if(expr->expression_type == EXPRESSION_NUM_CONST)
        printf("%d", expr->num_value);
    else if(expr->expression_type == EXPRESSION_STR_CONST)
        printf("\"%s\"", expr->str_value);
    else if(expr->expression_type == EXPRESSION_CHR_CONST)
        printf("'%s'", expr->str_value);

    /* Identifier */
    else if(expr->expression_type == EXPRESSION_IDENT)
        printf("%s", expr->str_value);

    /* Function call */
    else if(expr->expression_type == EXPRESSION_FUNCALL){
        printf("%s(", expr->str_value);
        for(int i = 0; i < expr->num_children; i++){
            print_expression(expr->children[i]);
            if(i != expr->num_children - 1)
                printf(", ");
        }
        printf(")");
    }

    /* Array access */
    else if(expr->expression_type == EXPRESSION_ARRAY_ACCESS){
        printf("%s[", expr->str_value);
        print_expression(expr->children[0]);
        printf("]");
    }

    /* Binary operators */
    else if(expr->expression_type == EXPRESSION_ARITH_ADD)
        print_binary_expression("+", expr);
    else if(expr->expression_type == EXPRESSION_ARITH_MUL)
        print_binary_expression("*", expr);
    else if(expr->expression_type == EXPRESSION_ARITH_SUB)
        print_binary_expression("-", expr);
    else if(expr->expression_type == EXPRESSION_ARITH_DIV)
        print_binary_expression("/", expr);
    else if(expr->expression_type == EXPRESSION_GREATER)
        print_binary_expression(">", expr);
    else if(expr->expression_type == EXPRESSION_GREATEREQ)
        print_binary_expression(">=", expr);
    else if(expr->expression_type == EXPRESSION_LESS)
        print_binary_expression("<", expr);
    else if(expr->expression_type == EXPRESSION_LESSEQ)
        print_binary_expression("<=", expr);
    else if(expr->expression_type == EXPRESSION_EQUALS)
        print_binary_expression("==", expr);
    else if(expr->expression_type == EXPRESSION_ASSIGN)
        print_binary_expression("=", expr);
    else if(expr->expression_type == EXPRESSION_OR)
        print_binary_expression("||", expr);
    else if(expr->expression_type == EXPRESSION_AND)
        print_binary_expression("&&", expr);

    /* Unary operators */
    else if(expr->expression_type == EXPRESSION_NOT)
        print_unary_expression("!", expr);
    else if(expr->expression_type == EXPRESSION_DEREF)
        print_unary_expression("*", expr);
    else if(expr->expression_type == EXPRESSION_ADDR)
        print_unary_expression("&", expr);
    else if(expr->expression_type == EXPRESSION_PREINCR)
        print_unary_expression("++", expr);
    else if(expr->expression_type == EXPRESSION_PREDECR)
        print_unary_expression("--", expr);

    /* Post increment and decrement */
    else if(expr->expression_type == EXPRESSION_POSTINCR){
        print_expression(expr->children[0]);
        printf("++");
    }
    else if(expr->expression_type == EXPRESSION_POSTDECR){
        print_expression(expr->children[0]);
        printf("--");
    }

    /* Catch-all unknown case */
    else {
        printf("Unknown(%d)", expr->expression_type);
    }
}

/* Release memory used by an expression */
void free_expression(expression* expr){
    /* Free string value */
    if(expr->str_value != NULL)
        free(expr->str_value);

    /* Free child expressions */
    for(int i = 0; i < expr->num_children; i++)
        free_expression(expr->children[i]);

    /* Free children array */
    if(expr->children != NULL)
        free(expr->children);

    /* Free memory itself */
    free(expr);
} 

/*** Expression constructors ***/

/* Create a numeric literal expression */
expression* numeric_constant_expression(int num){
    expression* expr = create_expression(EXPRESSION_NUM_CONST);
    expr->num_value = num;
    return expr;
}

/* Create a string literal expression */
expression* string_constant_expression(char* str){
    expression* expr = create_expression(EXPRESSION_STR_CONST);
    expr->str_value = str;
    return expr;
}

/* Create a character literal expression */
expression* char_constant_expression(char* c){
    expression* expr = create_expression(EXPRESSION_CHR_CONST);
    expr->str_value = c;
    return expr;
}

/* Create a numeric literal expression with the value 0 */
expression* zero_expression(){
    return numeric_constant_expression(0);
}

/* Create a unary operator expression */
expression* create_unary_expr(expression* ptr_expr, int tok_type){
    int expr_type = 0;
    if(tok_type == TOKEN_UNARY_DEREF)
        expr_type = EXPRESSION_DEREF;
    if(tok_type == TOKEN_ADDR)
        expr_type = EXPRESSION_ADDR;
    if(tok_type == TOKEN_NOT)
        expr_type = EXPRESSION_NOT;
    if(tok_type == TOKEN_POSTINCR)
        expr_type = EXPRESSION_POSTINCR;
    if(tok_type == TOKEN_POSTDECR)
        expr_type = EXPRESSION_POSTDECR;
    if(tok_type == TOKEN_PREINCR)
        expr_type = EXPRESSION_PREINCR;
    if(tok_type == TOKEN_PREDECR)
        expr_type = EXPRESSION_PREDECR;

    expression* expr = create_expression(expr_type);
    expr->num_children = 1;
    expr->children = calloc(1, sizeof(expression*));
    expr->children[0] = ptr_expr;
    return expr;
}

/* Create a binary operator expression */
expression* create_arithmetic_expr(expression* first, expression* second, int tok_type){
    int expr_type = 0;
    if(tok_type == TOKEN_PLUS)
        expr_type = EXPRESSION_ARITH_ADD;
    if(tok_type == TOKEN_MINUS)
        expr_type = EXPRESSION_ARITH_SUB;
    if(tok_type == TOKEN_TIMES)
        expr_type = EXPRESSION_ARITH_MUL;
    if(tok_type == TOKEN_DIV)
        expr_type = EXPRESSION_ARITH_DIV;
    if(tok_type == TOKEN_AND)
        expr_type = EXPRESSION_AND;
    if(tok_type == TOKEN_OR)
        expr_type = EXPRESSION_OR;
    if(tok_type == TOKEN_EQUALS)
        expr_type = EXPRESSION_EQUALS;
    if(tok_type == TOKEN_GREATER)
        expr_type = EXPRESSION_GREATER;
    if(tok_type == TOKEN_GREATEREQ)
        expr_type = EXPRESSION_GREATEREQ;
    if(tok_type == TOKEN_LESS)
        expr_type = EXPRESSION_LESS;
    if(tok_type == TOKEN_LESSEQ)
        expr_type = EXPRESSION_LESSEQ;

    expression* expr = create_expression(expr_type);
    expr->num_children = 2;
    expr->children = calloc(expr->num_children, sizeof(expression*));
    expr->children[0] = first;
    expr->children[1] = second;
    return expr;
}

/* Create a function call expression */
expression* create_funcall_expr(char* name, int args, expression** arg_exprs){
    expression* expr = create_expression(EXPRESSION_FUNCALL);
    expr->str_value = strdup(name);
    expr->num_children = args;
    expr->children = arg_exprs;
    return expr;
}

/* Create an array access expression */
expression* create_array_access_expr(char* name, expression* index_expr){
    expression* expr = create_expression(EXPRESSION_ARRAY_ACCESS);
    expr->str_value = strdup(name);
    expr->num_children = 1;
    expr->children = calloc(1, sizeof(expression*));
    expr->children[0] = index_expr;
    return expr;
}

/* Create a variable evaluation expression */
expression* create_var_expression(char* ident){
    expression* expr = create_expression(EXPRESSION_IDENT);
    expr->str_value = strdup(ident);
    return expr;
}

/*** Parsing functions ***/

/* Return 1 if the token represents a prefix or infix operator, 0 otherwise */
int is_operator_token(int tok_type){
    int operators[13] = {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_TIMES, TOKEN_DIV, 
        TOKEN_OR, TOKEN_AND, TOKEN_ASSIGN, TOKEN_NOT, 
        TOKEN_ADDR, TOKEN_UNARY_DEREF, TOKEN_UNARY_MINUS, TOKEN_PREINCR,
        TOKEN_PREDECR
    };
    for(int i = 0; i < 13; i++)
        if(tok_type == operators[i])
            return 1;

    return 0;
}

/* Check if an operator is left-associative */
int is_left_associative(int tok_type){
    /* Only a few operators are right associative - assignment, unary minus, dereferencing, and addressing */
    if(tok_type == TOKEN_ASSIGN || tok_type == TOKEN_UNARY_MINUS ||
            tok_type == TOKEN_UNARY_DEREF || tok_type == TOKEN_ADDR)
        return 0;
    else
        return 1;
}

/* 
 * Return a value representing operator precedence. Higher values correspond
 * to higher precedence. Thus, * will return a higher value than +.
 */
int op_precedence(int tok_type){
    if(tok_type == TOKEN_ASSIGN)
        return 1;
    if(tok_type == TOKEN_AND || tok_type == TOKEN_OR)
        return 5;
    if(tok_type == TOKEN_NOT)
        return 9;
    if(tok_type == TOKEN_PLUS || tok_type == TOKEN_MINUS)
        return 10;
    if(tok_type == TOKEN_TIMES || tok_type == TOKEN_DIV)
        return 20;
    if(tok_type == TOKEN_UNARY_DEREF || tok_type == TOKEN_UNARY_MINUS || tok_type == TOKEN_ADDR)
        return 100;

    error("Unknown operator precedence");
    return 0;
}

/* Increment the token pointer and make sure we're not out of bounds on the token stream */
void inc_ptr(int* index, int len){
    (*index)++;

    /* Make sure we're not out of bounds */
    if(*index >= len)
        error("Unexpected end of token stream.");
}

/* Parse a value literal */
expression* parse_const_expr(token** tokens, int* index, int len){
    expression* expr = NULL;

    /* Parse numbers, strings, and characters */
    int token_type = tokens[*index]->type;
    if(token_type == TOKEN_NUMBER)
        expr = numeric_constant_expression(atoi(tokens[*index]->data));
    if(token_type == TOKEN_STRING)
        expr = string_constant_expression(tokens[*index]->data);
    if(token_type == TOKEN_CHAR)
        expr = char_constant_expression(tokens[*index]->data);
    
    if(expr == NULL)
        error("Expected constant");

    return expr;
}

/* 
 * Collapse an operator on the operator stack by applying it to the arguments on the 
 * output stack, and then pushing the resulting expression back on the output stack
 *
 * Returns: resulting expression (same expression that is now on top of the output stack)
 */
expression* collapse_operator(stack* output_stack, stack* op_stack, int tok_type){
    expression* expr = NULL;
    /* Replace a unary minus with 0 - value */
    if(tok_type == TOKEN_UNARY_MINUS){
        expression* first = stack_pop(output_stack);
        expr = create_arithmetic_expr(zero_expression(), first, TOKEN_MINUS);
    }

    /* Apply unary operators to the value on top of the output stack */
    else if(tok_type == TOKEN_UNARY_DEREF || tok_type == TOKEN_NOT || 
            tok_type == TOKEN_ADDR || tok_type == TOKEN_PREDECR || tok_type == TOKEN_PREINCR){
        expression* first = stack_pop(output_stack);
        expr = create_unary_expr(first, tok_type);
    }

    /* Apply binary operators to the top two values on the output stack */
    else if(is_operator_token(tok_type)){
        expression* second = stack_pop(output_stack);
        expression* first = stack_pop(output_stack);
        expr = create_arithmetic_expr(first, second, tok_type);
    }

    /* Take used operator off the stack */
    stack_pop(op_stack);

    /* 
     * If we've generated an expression, put it on the output stack
     * to replace whatever expressions the operator used up
     */
    if(expr != NULL)
        stack_push(output_stack, expr);

    return expr;
}

/* 
 * Collapse all operators and values on the output stack and operator stack into one value, until
 * we reach one of the two provided operators on the operator stack. This function is used
 * when we encounter a closing bracket or parenthesis, or a comma delimiting function arguments.
 *
 * Returns: 1 if an expression was collapsed, 0 if nothing was done.
 */
int collapse_expression_stack_until(stack* output_stack, stack* op_stack, int first_end_token, int second_end_token){
    int collapsed = 0;

    /* Until we reach either of the desired tokens */
    token* peek = stack_peek(op_stack);
    while(peek != NULL && peek->type != first_end_token && peek->type != second_end_token){
        /* Collapse the operator on top */
        collapse_operator(output_stack, op_stack, peek->type);

        /* We've collapsed something */
        collapsed = 1;

        peek = stack_peek(op_stack);
    }

    return collapsed;
}

/*
 * Parse expressions using a modified Shunting-Yard algorithm.
 *
 * The Shunting-Yard algorithm uses two stacks for parsing, an output
 * stack (of expressions) and an operator stack (of tokens). The output
 * stack contains already-processed expressions, while the operator stack
 * contains operators which haven't been used yet. It loops
 * over the tokens until it reaches the end (semicolon), and processes
 * them one by one:
 *  - Literals: pushed directly onto the output stack
 *
 *  - Unary postfix operators: they are applied to the top expresssion 
 *      on the output stack, and the resulting expression is put on the
 *      top of the output stack.
 *
 *  - Identifiers:
 *      - Normal identifiers: pushed onto the output stack as a variable
 *      - Followed by a [ or (: pushed onto the operator stack, because
 *          the identifier actually represents an array or function being
 *          called or accessed
 *
 *  - Infix operators: Infix operators are collapsed as they can be,
 *      with certain rules being followed to preserve order of operations.
 *
 *  - Closing ] and ): Treated as the end of an expression, and the
 *      previous expression is collapsed and put on the output stack.
 *
 *  - Commas: Since these indicate the end of an argument, operators
 *      are collapsed until we reach the previous comma or opening (.
 *
 * See actual code below for details on each of the above.
 *
 * In the final stage, the algorithm reduces all the operators and
 * expressions into one by taking the top operator on the stack, applying
 * it to the expressions on the output stack, and putting the result back
 * on the output stack. After no operators are left, there should only
 * be one expression on the output stack, and that is the result of parsing.
 *
 * Main Source: http://en.wikipedia.org/wiki/Shunting-yard_algorithm
 */
expression* parse_expression(token** tokens, int* index, int len){
    /* Create the operator and output stack */
    stack* output_stack = make_stack();
    stack* op_stack = make_stack();
    
    /* 
     * Use a look-behind token to resolve some ambiguities with unary
     * dereferencing, minus, pre-incrementing, and post-incrementing.
     * Negative number indicates beginning of expression.
     */
    int previous_token_type = -1;
    
    /* Loop until end of input or a semicolon */
    while(*index < len && tokens[*index]->type != TOKEN_SEMICOLON){
        int token_type = tokens[*index]->type;

        /* 
         * Fix unary prefix operators. Unary operators can occur at
         * the beginning of an expression, after an operator, or
         * after an opening parenthesis or bracket.
         *
         * Unary operators which need to be corrected include: 
         *  - dereference (ambiguous with multiplication)
         *  - unary minus (ambiguous with subtraction)
         *  - increment (ambiguous with post-increment)
         *  - decrement (ambiguous with post-decrement)
         */
        int can_be_prefix_operator = (previous_token_type < 0 || 
                       is_operator_token(previous_token_type) || 
                       previous_token_type == TOKEN_OPAREN || 
                       previous_token_type == TOKEN_OBRACKET);
        if(token_type == TOKEN_TIMES && can_be_prefix_operator)
            tokens[*index]->type = TOKEN_UNARY_DEREF;
        else if(token_type == TOKEN_MINUS && can_be_prefix_operator)
            tokens[*index]->type = TOKEN_UNARY_MINUS;
        else if(token_type == TOKEN_INCR && can_be_prefix_operator)
            tokens[*index]->type = TOKEN_PREINCR;
        else if(token_type == TOKEN_DECR && can_be_prefix_operator)
            tokens[*index]->type = TOKEN_PREDECR;

        /* 
         * Any increments and decrements which are not pre-increments
         * or pre-decrements are classified as post-increments/decrements.
         */
        else if(token_type == TOKEN_INCR)
            tokens[*index]->type = TOKEN_POSTINCR;
        else if(token_type == TOKEN_DECR)
            tokens[*index]->type = TOKEN_POSTDECR;

        /* Propogate the fix to the token type variable */
        token_type = tokens[*index]->type;

        /* Literal values: push them to the output stack */
        if(token_type == TOKEN_NUMBER || token_type == TOKEN_STRING || token_type == TOKEN_CHAR){
            expression* const_expr = parse_const_expr(tokens, index, len);
            stack_push(output_stack, const_expr);
        }

        /* 
         * Postfix arithmetic: apply the postfix operator directly to the
         * expression on top of the output stack, and keep the expression there
         */
        else if(token_type == TOKEN_POSTINCR || token_type == TOKEN_POSTDECR)
            stack_push(output_stack, create_unary_expr(stack_pop(output_stack), token_type));

        /* Infix arithmetic */
        else if(is_operator_token(token_type)){
            /* Collapse all operators that can be collapsed. Operators that can't be collapsed
             * are ones with a lower precedence than the current operator. For instance,
             * in the expression 1 + 2 * 3, when we reach the multiplication operator, we can't
             * collapse the addition operator, because that would consume the 1 and 2.
             * If we collapsed it, the result would be (1 + 2) * 3, but if we only collapse 
             * operators with higher or equal precedence, we get 1 + (2*3). 
             *
             * Instead of explaining this badly, I'll just refer you to the Wikipedia page:
             *      http://en.wikipedia.org/wiki/Shunting-yard_algorithm
             * It has a wonderful diagram.
             */
            token* peek = stack_peek(op_stack);
            while(peek != NULL && is_operator_token(peek->type) &&
                    ((is_left_associative(token_type) && op_precedence(token_type) <= op_precedence(peek->type)) || 
                     (!is_left_associative(token_type) && op_precedence(token_type) < op_precedence(peek->type)))){

                collapse_operator(output_stack, op_stack, peek->type);
                peek = stack_peek(op_stack);
            }

            /* Push the new operator onto the operator stack */
            stack_push(op_stack, tokens[*index]);
        }

        /* 
         * Process an identifier as a variable, or as the
         * begining of a function call or array access.
         */
        else if(token_type == TOKEN_IDENT){
            token* ident_token = tokens[*index];
            int next_type = tokens[*index + 1]->type;

            /* Function call or array access */
            if(next_type == TOKEN_OPAREN || next_type == TOKEN_OBRACKET)
                stack_push(op_stack, ident_token);

            /* Normal identifier */
            else 
                stack_push(output_stack, create_var_expression(ident_token->data));
        }

        /* Push opening parentheses and brackets onto the operator stack */
        else if(token_type == TOKEN_OPAREN || token_type == TOKEN_OBRACKET)
            stack_push(op_stack, tokens[*index]);

        else if(token_type == TOKEN_CPAREN){
            /* Collapse the previous argument (see the TOKEN_COMMA case) */
            int has_arg = collapse_expression_stack_until(output_stack, op_stack, TOKEN_OPAREN, TOKEN_COMMA);

            /* 
             * The following argument calculation is done in case this turns
             * out to be the closing parenthesis of a function call.
             *
             * If nothing was collapsed, we have 0 arguments.
             * If something was collapsed, we have 1 + # of commas arguments.
             */
            int args = 1;
            if(!has_arg)
                args = 0;

            /* Pop all commas and the left parenthesis off the operator stack,
             * while counting the number of arguments to this function */
            token* peek;
            while((peek = stack_pop(op_stack))->type == TOKEN_COMMA)
                args++;

            /* 
             * If the previous token on the operator stack is an identifier,
             * then this is the closing parenthesis to a function call.
             */
            peek = stack_peek(op_stack);
            if(peek != NULL && peek->type == TOKEN_IDENT){
                /* Pop arguments off the output stack, store them in an array */
                expression** arg_exprs = calloc(args, sizeof(expression*));
                for(int i = args - 1; i >= 0; i--)
                    arg_exprs[i] = stack_pop(output_stack);
                
                /* Create the function call and put it on the output stack */
                token* func_name = stack_pop(op_stack);
                stack_push(output_stack, create_funcall_expr(func_name->data, args, arg_exprs));
            }
        }

        /* Array closing bracket */
        else if(token_type == TOKEN_CBRACKET){
            /* 
             * Collapse all the operators and expressions until we reach
             * the opening bracket. Now, the top expression will be the
             * index which we want to access.
             */
            collapse_expression_stack_until(output_stack, op_stack, TOKEN_OBRACKET, -1);

            /* Remove the opening bracket from the op stack */
            stack_pop(op_stack);

            /* Get the array name, and push the array access onto the output stack */
            token* array_name = stack_pop(op_stack);
            stack_push(output_stack, create_array_access_expr(array_name->data, stack_pop(output_stack)));
        }

        /* Function argument separator */
        else if(token_type == TOKEN_COMMA){
            /* 
             * Collapse all expressions until either the previous comma or the
             * opening parenthesis. This leaves the top expression on the 
             * output stack as the expression representing this current 
             * argument, and nothing more.
             */
            collapse_expression_stack_until(output_stack, op_stack, TOKEN_OPAREN, TOKEN_COMMA);

            /* 
             * Push the comma onto the stack - this is used to count
             * arguments when we process the closing parenthesis 
             */
            stack_push(op_stack, tokens[*index]);
        }

        /* Error: we don't know what to do! */
        else
            error("Unexpected token type");

        /* Advance to the next token */
        previous_token_type = token_type;
        inc_ptr(index, len);
    }

    /* While there are ore operators on the stack */
    while(stack_peek(op_stack) != NULL){
        /* Collapse the operator and use as many expressions as we need to */
        token* op = stack_peek(op_stack);
        collapse_operator(output_stack, op_stack, op->type);
    }

    /* Get the top expression on the stack as our output */
    expression* expr = stack_pop(output_stack);

    /* Nothing else should be on the output stack */
    if(stack_pop(output_stack) != NULL)
        error("Expression remaining on stack");

    /* Free the stacks used */
    free_stack(output_stack);
    free_stack(op_stack);

    return expr;
}
