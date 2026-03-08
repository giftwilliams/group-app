#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie.h"

// Map a character to a trie index (a=0 … z=25, space=26)
static int char_index(char c) {
    c = tolower(c);
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c == ' ')              return 26;
    return -1;  // unsupported character
}

static TrieNode* new_node(void) {
    TrieNode* node = (TrieNode*)calloc(1, sizeof(TrieNode));
    node->is_end   = 0;
    node->user_id  = -1;
    return node;
}

void trie_init(Trie* t) {
    t->root = new_node();
}

// Insert a lowercase username into the trie — O(L) where L = name length
void trie_insert(Trie* t, const char* name, int user_id) {
    TrieNode* cur = t->root;
    for (int i = 0; name[i] != '\0'; i++) {
        int idx = char_index(name[i]);
        if (idx < 0) continue;           // skip unsupported chars
        if (!cur->children[idx])
            cur->children[idx] = new_node();
        cur = cur->children[idx];
    }
    cur->is_end  = 1;
    cur->user_id = user_id;
}

// Recursive DFS to collect all user IDs under a trie subtree
static void collect_all(TrieNode* node,
                         int* result_ids, int* result_count, int max_results) {
    if (!node || *result_count >= max_results) return;

    if (node->is_end) {
        result_ids[(*result_count)++] = node->user_id;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        collect_all(node->children[i], result_ids, result_count, max_results);
    }
}

// Search by prefix — returns up to max_results matching user IDs
void trie_search(Trie* t, const char* prefix,
                 int* result_ids, int* result_count, int max_results) {
    *result_count = 0;
    TrieNode* cur = t->root;

    // Navigate to the end of the prefix
    for (int i = 0; prefix[i] != '\0'; i++) {
        int idx = char_index(prefix[i]);
        if (idx < 0 || !cur->children[idx]) return;  // prefix not found
        cur = cur->children[idx];
    }

    // Collect all names under this prefix node
    collect_all(cur, result_ids, result_count, max_results);
}

// Free all trie nodes recursively
static void free_node(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        free_node(node->children[i]);
    free(node);
}

void trie_free(Trie* t) {
    free_node(t->root);
    t->root = NULL;
}
