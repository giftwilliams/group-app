#ifndef TRIE_H
#define TRIE_H

#include "hashtable.h"

#define ALPHABET_SIZE 27   // a-z + space

// Each trie node
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    int              is_end;       // marks end of a username
    int              user_id;      // ID of the user whose name ends here
} TrieNode;

// Trie root wrapper
typedef struct {
    TrieNode* root;
} Trie;

void trie_init(Trie* t);
void trie_insert(Trie* t, const char* name, int user_id);
void trie_search(Trie* t, const char* prefix,
                 int* result_ids, int* result_count, int max_results);
void trie_free(Trie* t);

#endif
