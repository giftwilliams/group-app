#ifndef HEAP_H
#define HEAP_H

#include "hashtable.h"

#define HEAP_MAX 500

// Max-heap ranked by friend_count
typedef struct {
    User* data[HEAP_MAX];
    int   size;
} Heap;

void  heap_init(Heap* h);
void  heap_insert(Heap* h, User* user);
User* heap_extract_max(Heap* h);
void  heap_get_topk(Heap* h, User** out, int k, int* found);

#endif
