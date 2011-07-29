#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exit with an error */
void error(char* error){
    fprintf(stderr, "%s\n", error);
    /* Cause segfault */
    *((int*)(NULL)) = 0;
    exit(1);
}

/* Increment the token pointer and make sure we're not out of bounds on the token stream */
void inc_ptr(int* index, int len){
    (*index)++;

    /* Make sure we're not out of bounds */
    if(*index >= len)
        error("Unexpected end of token stream.");
}

/* Duplicate a string */
char *strdup (char *s) {
    char *d = (char *)(malloc (strlen (s) + 1)); // Allocate memory
    if (d != NULL)
        strcpy (d,s);                            // Copy string if okay
    return d;                                    // Return new memory
}

/* Get the length of the file (number of characters) */
int file_get_length(char* fname){
    /* Open the file */
    FILE* file = fopen(fname, "r");

    /* Seek to the end */
    fseek(file, 0, SEEK_END);
    
    /* Get current position (since we're at the end, this is the file size) */
    long len = ftell(file);

    /* Close the file, return the length */
    fclose(file);
    return (int) len;
}

/* Read contents of text file */
char* read_file(char* fname){
    /* Get file length */
    long len = file_get_length(fname);

    /* Open file for reading */
    FILE* file = fopen(fname, "r");

    /* Allocate buffer for file data */
    char* buffer = (char*) calloc (1, sizeof(char)*len+1);

    /* Read data into buffer */
    int result = fread(buffer,1,len,file);

    /* Close file and return buffer */
    fclose (file);
    return buffer;
}

/* Stack */
typedef struct _stack {
    int size;
    int num;
    void** vals;
} stack;

typedef struct _queue {
    int size;
    int num;
    void** vals;
} queue;

stack* make_stack(){
    stack* stk = calloc(1, sizeof(stack));
    stk->size = 10;
    stk->vals = calloc(stk->size, sizeof(void*));
    return stk;
}

void stack_push(stack* stk, void* ptr){
    if(stk->num == stk->size){
        stk->size += 20;
        stk->vals = realloc(stk->vals, stk->size * sizeof(void*));
    }

    stk->vals[stk->num] = ptr;
    stk->num++; 
}

void* stack_pop(stack* stk){
    if(stk->num == 0)
        return NULL;

    stk->num--;
    void* ptr = stk->vals[stk->num];
    return ptr;
}

void* stack_peek(stack* stk){
    if(stk->num == 0)
        return NULL;

    void* ptr = stk->vals[stk->num-1];
    return ptr;
}

void free_stack(stack* stk){
    free(stk->vals);
    free(stk);
}

queue* make_queue(){
    queue* q = calloc(1, sizeof(queue));
    q->size = 10;
    q->vals = calloc(q->size, sizeof(void*));
    return q;
}

void enqueue(queue* q, void* ptr){
    if(ptr == NULL)
        error("Queueing null");

    if(q->num == q->size){
        q->size += 20;
        q->vals = realloc(q->vals, q->size * sizeof(void*));
    }

    q->vals[q->num] = ptr;
    q->num++; 
}

void* dequeue(queue* q){
    if(q->num == 0)
        return NULL;

    q->num--;
    void* ptr = q->vals[0];
    for(int i = 0; i < q->num; i++)
        q->vals[i] = q->vals[i+1];
    return ptr;
}

void free_queue(queue* q){
    free(q->vals);
    free(q);
}


