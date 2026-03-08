#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

void heap_init(Heap* h) {
    h->size = 0;
}

static void swap(User** a, User** b) {
    User* tmp = *a; *a = *b; *b = tmp;
}

// Bubble up after insert
static void heapify_up(Heap* h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (h->data[parent]->friend_count < h->data[idx]->friend_count) {
            swap(&h->data[parent], &h->data[idx]);
            idx = parent;
        } else break;
    }
}

// Bubble down after extract
static void heapify_down(Heap* h, int idx) {
    int largest = idx;
    int left    = 2 * idx + 1;
    int right   = 2 * idx + 2;

    if (left  < h->size && h->data[left]->friend_count  > h->data[largest]->friend_count) largest = left;
    if (right < h->size && h->data[right]->friend_count > h->data[largest]->friend_count) largest = right;

    if (largest != idx) {
        swap(&h->data[largest], &h->data[idx]);
        heapify_down(h, largest);
    }
}

// Insert a user pointer into the heap — O(log n)
void heap_insert(Heap* h, User* user) {
    if (h->size >= HEAP_MAX) return;
    h->data[h->size++] = user;
    heapify_up(h, h->size - 1);
}

// Extract the user with the most friends — O(log n)
User* heap_extract_max(Heap* h) {
    if (h->size == 0) return NULL;
    User* top = h->data[0];
    h->data[0] = h->data[--h->size];
    heapify_down(h, 0);
    return top;
}

// Get top-k users by friend count without permanently destroying heap
void heap_get_topk(Heap* h, User** out, int k, int* found) {
    // Copy the heap so we don't destroy the original
    Heap temp = *h;
    *found = 0;
    while (*found < k && temp.size > 0) {
        out[(*found)++] = heap_extract_max(&temp);
    }
}
