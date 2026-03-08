#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALPHABET_SIZE 26
#define MAX_WORD 100

// Trie Node
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int isEndOfWord;
} TrieNode;


// Create new Trie node
TrieNode* createNode() {
    TrieNode *node = (TrieNode*)malloc(sizeof(TrieNode));

    node->isEndOfWord = 0;

    for(int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;

    return node;
}


// Insert a name into the Trie
void insert(TrieNode *root, char *word) {

    TrieNode *current = root;

    for(int i = 0; word[i]; i++) {

        char c = tolower(word[i]);
        int index = c - 'a';

        if(index < 0 || index >= 26)
            continue;

        if(current->children[index] == NULL)
            current->children[index] = createNode();

        current = current->children[index];
    }

    current->isEndOfWord = 1;
}


// Print suggestions recursively
void printSuggestions(TrieNode *root, char *prefix) {

    if(root->isEndOfWord)
        printf("%s\n", prefix);

    for(int i = 0; i < ALPHABET_SIZE; i++) {

        if(root->children[i] != NULL) {

            char next[MAX_WORD];
            strcpy(next, prefix);

            int len = strlen(prefix);
            next[len] = 'a' + i;
            next[len+1] = '\0';

            printSuggestions(root->children[i], next);
        }
    }
}


// Autocomplete function
void autocomplete(TrieNode *root, char *prefix) {

    TrieNode *current = root;

    for(int i = 0; prefix[i]; i++) {

        char c = tolower(prefix[i]);
        int index = c - 'a';

        if(current->children[index] == NULL) {
            printf("No friends found with this prefix.\n");
            return;
        }

        current = current->children[index];
    }

    printf("\nSuggestions:\n");
    printSuggestions(current, prefix);
}


// Menu
void menu() {
    printf("\n===== FRIEND SEARCH SYSTEM =====\n");
    printf("1. Add Friend\n");
    printf("2. Search Friend (Autocomplete)\n");
    printf("3. Exit\n");
    printf("Choose option: ");
}


// Main Program
int main() {

    TrieNode *root = createNode();

    int choice;
    char name[MAX_WORD];

    while(1) {

        menu();
        scanf("%d", &choice);

        switch(choice) {

            case 1:

                printf("Enter friend name: ");
                scanf("%s", name);

                insert(root, name);
                printf("Friend added successfully.\n");

                break;

            case 2:

                printf("Enter search prefix: ");
                scanf("%s", name);

                autocomplete(root, name);

                break;

            case 3:
                printf("Exiting program...\n");
                exit(0);

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}SA
