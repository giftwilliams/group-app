#ifndef HASHTABLE_H
#define HASHTABLE_H

#define TABLE_SIZE   512
#define MAX_NAME_LEN  50

// A single user profile
typedef struct User {
    int  id;
    char name[MAX_NAME_LEN];
    int  friend_count;   // used by heap for ranking
} User;

// Chaining node for collision handling
typedef struct HTNode {
    User        user;
    struct HTNode* next;
} HTNode;

// Hash table
typedef struct {
    HTNode* buckets[TABLE_SIZE];
    int     count;
} HashTable;

void   ht_init(HashTable* ht);
int    ht_insert(HashTable* ht, int id, const char* name);
User*  ht_search(HashTable* ht, int id);
void   ht_delete(HashTable* ht, int id);
void   ht_print_all(HashTable* ht);
void   ht_free(HashTable* ht);

#endif
