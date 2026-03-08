#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

// Simple hash function: sum of (char * position) mod TABLE_SIZE
static unsigned int hash(int id) {
    return (unsigned int)id % TABLE_SIZE;
}

void ht_init(HashTable* ht) {
    for (int i = 0; i < TABLE_SIZE; i++) ht->buckets[i] = NULL;
    ht->count = 0;
}

// Insert a new user; returns 0 on success, -1 if ID already exists
int ht_insert(HashTable* ht, int id, const char* name) {
    unsigned int idx = hash(id);
    HTNode* cur = ht->buckets[idx];

    // Check for duplicate ID
    while (cur) {
        if (cur->user.id == id) return -1;
        cur = cur->next;
    }

    HTNode* node = (HTNode*)malloc(sizeof(HTNode));
    node->user.id           = id;
    node->user.friend_count = 0;
    strncpy(node->user.name, name, MAX_NAME_LEN - 1);
    node->user.name[MAX_NAME_LEN - 1] = '\0';
    node->next              = ht->buckets[idx];
    ht->buckets[idx]        = node;
    ht->count++;
    return 0;
}

// Search by user ID — O(1) average
User* ht_search(HashTable* ht, int id) {
    unsigned int idx = hash(id);
    HTNode* cur = ht->buckets[idx];
    while (cur) {
        if (cur->user.id == id) return &cur->user;
        cur = cur->next;
    }
    return NULL;
}

void ht_delete(HashTable* ht, int id) {
    unsigned int idx = hash(id);
    HTNode* cur  = ht->buckets[idx];
    HTNode* prev = NULL;
    while (cur) {
        if (cur->user.id == id) {
            if (prev) prev->next = cur->next;
            else      ht->buckets[idx] = cur->next;
            free(cur);
            ht->count--;
            return;
        }
        prev = cur;
        cur  = cur->next;
    }
}

void ht_print_all(HashTable* ht) {
    printf("  %-6s %-25s %s\n", "ID", "Name", "Friends");
    printf("  ------ ------------------------- -------\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        HTNode* cur = ht->buckets[i];
        while (cur) {
            printf("  %-6d %-25s %d\n",
                   cur->user.id, cur->user.name, cur->user.friend_count);
            cur = cur->next;
        }
    }
}

void ht_free(HashTable* ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HTNode* cur = ht->buckets[i];
        while (cur) {
            HTNode* tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        ht->buckets[i] = NULL;
    }
    ht->count = 0;
}
