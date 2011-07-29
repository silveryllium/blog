/* A type type, which defines a primitive or user-specified type. It can be
 * a pointer to another type, or a base type. Base types have a name ("int", 
 * "char", "void", etc) and a size (respectively, 4, 1, 0, etc).
 */
typedef struct _type {
    struct _type* ptr_to;
    struct _type* alias;
    int size;
    char* name;

    int struct_type;
    int num_fields;
    struct _type** field_types;
    char** field_names;
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
    if(type->ptr_to == NULL) {
        if(!type->struct_type)
            printf("%s", type->name);
        else {
            printf("struct %s { ", type->name == NULL ? "" : type->name);
            for(int i = 0; i < type->num_fields; i++){
                print_type(type->field_types[i]);
                printf(" ");
                printf("%s", type->field_names[i]);
                printf("; ");
            }
            printf(" }");
        }
    } else {
        if(type->ptr_to->struct_type){
            printf("struct %s", type->ptr_to->name == NULL ? "" : type->ptr_to->name);
        } else
            print_type(type->ptr_to);
        printf("*");
    }
}

/* Free memory used by a type */
void free_type(type* type){
    if(type->struct_type)
        return;

    if(type->ptr_to != NULL)
        free_type(type->ptr_to);
    if(type->name != NULL)
        free(type->name);
    free(type);
}

void free_struct_type(type* type){
    type->struct_type = 0;
    free_type(type);
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
    for(int i = 0; i < type_table.num; i++){
        if(type_table.types[i]->struct_type)
            free_struct_type(type_table.types[i]);
        else
            free_type(type_table.types[i]);
    }
    free(type_table.types);
}

/* Parse type information */
type* parse_type(token** tokens, int* index, int len){
    int first_type = tokens[*index]->type;
    char* token_str = tokens[*index]->data;
    inc_ptr(index, len);

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

    /* If this is a struct, parse it */
    else if(first_type == TOKEN_STRUCT){
        type* struct_type;

        /* If it's a named struct, we have to add it to our known list */
        if(tokens[*index]->type == TOKEN_IDENT){
            for(int i = 0; i < type_table.num; i++){
                if(type_table.types[i]->struct_type && strcmp(type_table.types[i]->name, tokens[*index]->data) == 0)
                    base = type_table.types[i];
            }

            if(base == NULL){
                struct_type = create_type(0);
                struct_type->struct_type = 1;
                struct_type->name = strdup(tokens[*index]->data);
                add_to_type_table(struct_type);
            }
            inc_ptr(index, len);
        }

        if(base == NULL) {
            if(tokens[*index]->type != TOKEN_OBRACE)
                error("Expected opening brace");
            else
                inc_ptr(index, len);

            while(tokens[*index]->type != TOKEN_CBRACE){
                type* field_type = parse_type(tokens, index, len);
                if(tokens[*index]->type != TOKEN_IDENT)
                    error("Expected field name identifier");
                char* ident = strdup(tokens[*index]->data);
                inc_ptr(index, len);

                if(tokens[*index]->type != TOKEN_SEMICOLON)
                    error("Expected semicolon after field declaration");
                inc_ptr(index, len);

                int field_ind = struct_type->num_fields;
                struct_type->size += field_type->size;
                struct_type->num_fields++;
                struct_type->field_types = realloc(struct_type->field_types, struct_type->num_fields * sizeof(type*));
                struct_type->field_names = realloc(struct_type->field_names, struct_type->num_fields * sizeof(char*));
                struct_type->field_types[field_ind] = field_type;
                struct_type->field_names[field_ind] = ident;
            }
            inc_ptr(index, len);

            /* Replace copied instances of this type with new copied instances (updated) */
            for(int i = 0; i < struct_type->num_fields; i++){
                type* ftype = struct_type->field_types[i];
                if(ftype->ptr_to != NULL)
                    ftype = ftype->ptr_to;
                else
                    continue;

                if(ftype->struct_type && struct_type->name != NULL && ftype->name != NULL 
                        && strcmp(ftype->name, struct_type->name) == 0){
                    free_type(struct_type->field_types[i]);
                    struct_type->field_types[i] = create_type_ptr(struct_type);
                }
            }

            base = struct_type;
        }
    }

    if(base == NULL){
        (*index)--;
        return NULL;
    }

    /* For every asterisk after the base type, wrap the type in a pointer type */
    while(tokens[*index]->type == TOKEN_TIMES){
        base = create_type_ptr(base);
        inc_ptr(index, len);
    }

    return base;
}
