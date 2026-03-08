#ifndef STACK_H
#define STACK_H

#define MAX_HISTORY 5
#define MAX_QUERY_LEN 50

// Stack node for search history
typedef struct StackNode {
    char query[MAX_QUERY_LEN];
    struct StackNode* next;
} StackNode;

// Stack structure
typedef struct {
    StackNode* top;
    int size;
} Stack;

void     stack_init(Stack* s);
void     stack_push(Stack* s, const char* query);
char*    stack_peek(Stack* s);
void     stack_pop(Stack* s);
void     stack_print_history(Stack* s);
void     stack_free(Stack* s);

#endif
