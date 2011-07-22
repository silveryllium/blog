/* A type type, which defines a primitive or user-specified type. It can be
 * a pointer to another type, or a base type. Base types have a name ("int", 
 * "char", "void", etc) and a size (respectively, 4, 1, 0, etc).
 */
typedef struct _type {
    struct _type* ptr_to;
    int size;
    char* name;
} type;

/* Global type table, stores array of known types */
struct {
    int num;
    type** types;
} type_table;

/* All the primitive built-in types and their indices in the type table */
int TYPE_VOID = 0;
int TYPE_CHAR = 1;
int TYPE_INT = 2;

/* Add a type to the global type table */
void add_to_type_table(type* t){
    type_table.num++;
    type_table.types = realloc(type_table.types, type_table.num * sizeof(type*));
    type_table.types[type_table.num - 1] = t;
}

/* Create a blank type given size in bytes */
type* create_type(int size){
    type* t = calloc(1, sizeof(type));
    t->size = size;
    return t;
}

/* Print the type in some useful debugging format to stdout */
void print_type(type* type){
    if(type->ptr_to == NULL)
        printf("%s", type->name);
    else {
        print_type(type->ptr_to);
        printf("*");
    }
}

/* Free memory used by a type */
void free_type(type* type){
    if(type->ptr_to != NULL)
        free_type(type->ptr_to);
    if(type->name != NULL)
        free(type->name);
    free(type);
}

/* Copy a type */
type* copy_type(type* t){
    type* cpy = create_type(t->size);
    if(t->ptr_to != NULL)
        cpy->ptr_to = copy_type(t->ptr_to);
    else
        cpy->name = strdup(t->name);

    return cpy;
}

/* Create a type which is a pointer to another type */
type* create_type_ptr(type* point_to){
    /* All pointers are 4 bytes (on 32-bit systems, which we're targeting) */
    type* type = create_type(4);
    type->ptr_to = point_to;
    return type;
}

/* Initialize the type table by adding primitive types */
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

/* Delete the type table and any memory it uses */
void del_type_table(){
    for(int i = 0; i < type_table.num; i++)
        free_type(type_table.types[i]);
    free(type_table.types);
}

/* Parse type information */
type* parse_type(token** tokens, int* index, int len){
    int first_type = tokens[*index]->type;
    char* token_str = tokens[*index]->data;
    (*index)++;

    type* base = NULL;

    /* If this is a primitive type, return a copy of it from the type table */
    if(first_type == TOKEN_INT)
        base = copy_type(type_table.types[TYPE_INT]);
    else if(first_type == TOKEN_CHAR)
        base = copy_type(type_table.types[TYPE_CHAR]);
    else if(first_type == TOKEN_VOID)
        base = copy_type(type_table.types[TYPE_VOID]);

    /* If this is a known user-defined type, loop through the type table and find it */
    else if(first_type == TOKEN_IDENT){
        for(int i = 0; i < type_table.num; i++){
            if(type_table.types[i]->name != NULL && strcmp(type_table.types[i]->name, token_str) == 0){
                base = copy_type(type_table.types[i]);
                break;
            }
        }
    }

    if(base == NULL)
        error("Unknown type");

    /* For every asterisk after the base type, wrap the type in a pointer type */
    while(tokens[*index]->type == TOKEN_TIMES){
        base = create_type_ptr(base);
        (*index)++;
    }

    return base;
}
