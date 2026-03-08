#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

void stack_init(Stack* s) {
    s->top  = NULL;
    s->size = 0;
}

// Push a search query; if history exceeds MAX_HISTORY, drop the oldest
void stack_push(Stack* s, const char* query) {
    // If already at max, remove the bottom node
    if (s->size == MAX_HISTORY) {
        // Traverse to second-to-last node
        StackNode* cur = s->top;
        if (s->size == 1) {
            free(s->top);
            s->top = NULL;
            s->size = 0;
        } else {
            while (cur->next && cur->next->next) cur = cur->next;
            free(cur->next);
            cur->next = NULL;
            s->size--;
        }
    }

    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    strncpy(node->query, query, MAX_QUERY_LEN - 1);
    node->query[MAX_QUERY_LEN - 1] = '\0';
    node->next = s->top;
    s->top     = node;
    s->size++;
}

char* stack_peek(Stack* s) {
    if (!s->top) return NULL;
    return s->top->query;
}

void stack_pop(Stack* s) {
    if (!s->top) return;
    StackNode* tmp = s->top;
    s->top = s->top->next;
    free(tmp);
    s->size--;
}

void stack_print_history(Stack* s) {
    if (!s->top) {
        printf("  (no search history)\n");
        return;
    }
    StackNode* cur = s->top;
    int i = 1;
    while (cur) {
        printf("  %d. %s\n", i++, cur->query);
        cur = cur->next;
    }
}

void stack_free(Stack* s) {
    while (s->top) stack_pop(s);
}
