/*
 * ============================================================
 *  ConnectX — Scalable Friend Search & Autocomplete
 *  DSA-CH23 | Group Project | Variant A5
 *  All code in one file for simplicity
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ============================================================
   CONSTANTS
   ============================================================ */
#define TABLE_SIZE    512
#define MAX_NAME_LEN   50
#define MAX_QUERY_LEN  50
#define MAX_HISTORY     5
#define ALPHABET_SIZE  27   // a-z + space
#define HEAP_MAX      500
#define MAX_USERS     500
#define MAX_FRIENDS    50

/* ============================================================
   USER PROFILE (used everywhere)
   ============================================================ */
typedef struct User {
    int  id;
    char name[MAX_NAME_LEN];
    int  friend_count;
} User;

/* ============================================================
   1. HASH TABLE — stores user profiles
   ============================================================ */
typedef struct HTNode {
    User        user;
    struct HTNode* next;
} HTNode;

typedef struct {
    HTNode* buckets[TABLE_SIZE];
    int     count;
} HashTable;

static unsigned int ht_hash(int id) {
    return (unsigned int)id % TABLE_SIZE;
}

void ht_init(HashTable* ht) {
    for (int i = 0; i < TABLE_SIZE; i++) ht->buckets[i] = NULL;
    ht->count = 0;
}

int ht_insert(HashTable* ht, int id, const char* name) {
    unsigned int idx = ht_hash(id);
    HTNode* cur = ht->buckets[idx];
    while (cur) {
        if (cur->user.id == id) return -1;
        cur = cur->next;
    }
    HTNode* node = (HTNode*)malloc(sizeof(HTNode));
    node->user.id           = id;
    node->user.friend_count = 0;
    strncpy(node->user.name, name, MAX_NAME_LEN - 1);
    node->user.name[MAX_NAME_LEN - 1] = '\0';
    node->next         = ht->buckets[idx];
    ht->buckets[idx]   = node;
    ht->count++;
    return 0;
}

User* ht_search(HashTable* ht, int id) {
    unsigned int idx = ht_hash(id);
    HTNode* cur = ht->buckets[idx];
    while (cur) {
        if (cur->user.id == id) return &cur->user;
        cur = cur->next;
    }
    return NULL;
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
        while (cur) { HTNode* t = cur; cur = cur->next; free(t); }
        ht->buckets[i] = NULL;
    }
}

/* ============================================================
   2. TRIE — autocomplete by name prefix
   ============================================================ */
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    int              is_end;
    int              user_id;
} TrieNode;

typedef struct { TrieNode* root; } Trie;

static int char_index(char c) {
    c = tolower(c);
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c == ' ')              return 26;
    return -1;
}

static TrieNode* trie_new_node(void) {
    TrieNode* n = (TrieNode*)calloc(1, sizeof(TrieNode));
    n->user_id  = -1;
    return n;
}

void trie_init(Trie* t) { t->root = trie_new_node(); }

void trie_insert(Trie* t, const char* name, int user_id) {
    TrieNode* cur = t->root;
    for (int i = 0; name[i]; i++) {
        int idx = char_index(name[i]);
        if (idx < 0) continue;
        if (!cur->children[idx]) cur->children[idx] = trie_new_node();
        cur = cur->children[idx];
    }
    cur->is_end  = 1;
    cur->user_id = user_id;
}

static void trie_collect(TrieNode* node, int* ids, int* cnt, int max) {
    if (!node || *cnt >= max) return;
    if (node->is_end) ids[(*cnt)++] = node->user_id;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        trie_collect(node->children[i], ids, cnt, max);
}

void trie_search(Trie* t, const char* prefix, int* ids, int* cnt, int max) {
    *cnt = 0;
    TrieNode* cur = t->root;
    for (int i = 0; prefix[i]; i++) {
        int idx = char_index(prefix[i]);
        if (idx < 0 || !cur->children[idx]) return;
        cur = cur->children[idx];
    }
    trie_collect(cur, ids, cnt, max);
}

static void trie_free_node(TrieNode* n) {
    if (!n) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) trie_free_node(n->children[i]);
    free(n);
}
void trie_free(Trie* t) { trie_free_node(t->root); t->root = NULL; }

/* ============================================================
   3. STACK — search history (last 5 searches)
   ============================================================ */
typedef struct StackNode {
    char query[MAX_QUERY_LEN];
    struct StackNode* next;
} StackNode;

typedef struct { StackNode* top; int size; } Stack;

void stack_init(Stack* s) { s->top = NULL; s->size = 0; }

void stack_push(Stack* s, const char* query) {
    if (s->size == MAX_HISTORY) {
        StackNode* cur = s->top;
        if (s->size == 1) { free(s->top); s->top = NULL; s->size = 0; }
        else {
            while (cur->next && cur->next->next) cur = cur->next;
            free(cur->next); cur->next = NULL; s->size--;
        }
    }
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    strncpy(node->query, query, MAX_QUERY_LEN - 1);
    node->query[MAX_QUERY_LEN - 1] = '\0';
    node->next = s->top; s->top = node; s->size++;
}

void stack_print(Stack* s) {
    if (!s->top) { printf("  (no search history)\n"); return; }
    StackNode* cur = s->top; int i = 1;
    while (cur) { printf("  %d. %s\n", i++, cur->query); cur = cur->next; }
}

void stack_free(Stack* s) {
    while (s->top) { StackNode* t = s->top; s->top = s->top->next; free(t); s->size--; }
}

/* ============================================================
   4. MAX-HEAP — top-k users by friend count
   ============================================================ */
typedef struct { User* data[HEAP_MAX]; int size; } Heap;

void heap_init(Heap* h) { h->size = 0; }

static void heap_swap(User** a, User** b) { User* t = *a; *a = *b; *b = t; }

static void heapify_up(Heap* h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->data[p]->friend_count < h->data[i]->friend_count)
            { heap_swap(&h->data[p], &h->data[i]); i = p; }
        else break;
    }
}

static void heapify_down(Heap* h, int i) {
    int largest = i, l = 2*i+1, r = 2*i+2;
    if (l < h->size && h->data[l]->friend_count > h->data[largest]->friend_count) largest = l;
    if (r < h->size && h->data[r]->friend_count > h->data[largest]->friend_count) largest = r;
    if (largest != i) { heap_swap(&h->data[largest], &h->data[i]); heapify_down(h, largest); }
}

void heap_insert(Heap* h, User* u) {
    if (h->size >= HEAP_MAX) return;
    h->data[h->size++] = u; heapify_up(h, h->size - 1);
}

static User* heap_extract_max(Heap* h) {
    if (!h->size) return NULL;
    User* top = h->data[0];
    h->data[0] = h->data[--h->size]; heapify_down(h, 0);
    return top;
}

void heap_topk(Heap* h, User** out, int k, int* found) {
    Heap tmp = *h; *found = 0;
    while (*found < k && tmp.size > 0) out[(*found)++] = heap_extract_max(&tmp);
}

/* ============================================================
   5. GRAPH — friend connections + BFS mutual friends
   ============================================================ */
typedef struct {
    int adj[MAX_USERS][MAX_FRIENDS];
    int degree[MAX_USERS];
} Graph;

void graph_init(Graph* g) {
    memset(g->adj, 0, sizeof(g->adj));
    memset(g->degree, 0, sizeof(g->degree));
}

int graph_add_friend(Graph* g, int a, int b) {
    if (a < 0 || a >= MAX_USERS || b < 0 || b >= MAX_USERS) return -1;
    if (g->degree[a] >= MAX_FRIENDS || g->degree[b] >= MAX_FRIENDS) return -1;
    for (int i = 0; i < g->degree[a]; i++) if (g->adj[a][i] == b) return -1;
    g->adj[a][g->degree[a]++] = b;
    g->adj[b][g->degree[b]++] = a;
    return 0;
}

void graph_mutual(Graph* g, int a, int b, int* mutual, int* mc) {
    *mc = 0;
    int visited[MAX_USERS] = {0};
    for (int i = 0; i < g->degree[a]; i++) visited[g->adj[a][i]] = 1;
    for (int i = 0; i < g->degree[b]; i++) {
        int f = g->adj[b][i];
        if (visited[f] && f != a) mutual[(*mc)++] = f;
    }
}

/* ============================================================
   DEMO DATA + MENU
   ============================================================ */
static void print_banner(void) {
    printf("\n");
    printf("  ██████╗ ██████╗ ███╗   ██╗███╗   ██╗███████╗ ██████╗████████╗██╗  ██╗\n");
    printf(" ██╔════╝██╔═══██╗████╗  ██║████╗  ██║██╔════╝██╔════╝╚══██╔══╝╚██╗██╔╝\n");
    printf(" ██║     ██║   ██║██╔██╗ ██║██╔██╗ ██║█████╗  ██║        ██║    ╚███╔╝ \n");
    printf(" ██║     ██║   ██║██║╚██╗██║██║╚██╗██║██╔══╝  ██║        ██║    ██╔██╗ \n");
    printf(" ╚██████╗╚██████╔╝██║ ╚████║██║ ╚████║███████╗╚██████╗   ██║   ██╔╝ ██╗\n");
    printf("  ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═══╝╚══════╝ ╚═════╝   ╚═╝   ╚═╝  ╚═╝\n");
    printf("\n  Scalable Friend Search & Autocomplete — DSA-CH23 Group Project (A5)\n");
    printf("  -----------------------------------------------------------------------\n\n");
}

static void print_menu(void) {
    printf("\n  ┌──────────────────────────────────────┐\n");
    printf("  │           ConnectX  MENU             │\n");
    printf("  ├──────────────────────────────────────┤\n");
    printf("  │  1. Register a user                  │\n");
    printf("  │  2. Search users by prefix           │\n");
    printf("  │  3. Add friendship                   │\n");
    printf("  │  4. Find mutual friends              │\n");
    printf("  │  5. Show top-5 users (most friends)  │\n");
    printf("  │  6. View search history              │\n");
    printf("  │  7. List all users                   │\n");
    printf("  │  8. Run benchmark                    │\n");
    printf("  │  0. Exit                             │\n");
    printf("  └──────────────────────────────────────┘\n");
    printf("  Choice: ");
}

static void seed_demo(HashTable* ht, Trie* t, Graph* g, Heap* h) {
    const char* names[] = {
        "Alice Johnson","Bob Marley","Charlie Brown","Diana Prince",
        "Edward Norton","Fatima Hassan","George Clooney","Hannah White",
        "Ivan Petrov","Julia Roberts","Kevin Hart","Laura Croft",
        "Mohammed Ali","Nina Simone","Oscar Wilde","Paula Abdul",
        "Quinn Hughes","Rachel Green","Samuel Adams","Tina Turner"
    };
    for (int i = 0; i < 20; i++) {
        ht_insert(ht, i, names[i]);
        trie_insert(t, names[i], i);
        User* u = ht_search(ht, i);
        if (u) heap_insert(h, u);
    }
    int pairs[][2] = {
        {0,1},{0,2},{0,5},{1,2},{1,3},{2,4},{3,4},{3,5},
        {5,6},{6,7},{7,8},{8,9},{9,10},{10,11},{11,12},
        {0,3},{4,6},{7,9},{12,13},{14,15}
    };
    for (int i = 0; i < 20; i++) {
        int a = pairs[i][0], b = pairs[i][1];
        if (graph_add_friend(g, a, b) == 0) {
            User* ua = ht_search(ht, a);
            User* ub = ht_search(ht, b);
            if (ua) ua->friend_count++;
            if (ub) ub->friend_count++;
        }
    }
    printf("  [✓] Loaded 20 demo users with sample friendships.\n");
}

static void run_benchmark(Trie* t, HashTable* ht) {
    printf("\n  --- Benchmark ---\n");
    int ids[10], cnt;
    clock_t s = clock();
    for (int i = 0; i < 10000; i++) trie_search(t, "a", ids, &cnt, 10);
    printf("  Trie search  x10,000 : %.3f ms\n", 1000.0*(clock()-s)/CLOCKS_PER_SEC);
    s = clock();
    for (int i = 0; i < 10000; i++) ht_search(ht, i % 20);
    printf("  Hash lookup  x10,000 : %.3f ms\n", 1000.0*(clock()-s)/CLOCKS_PER_SEC);
    printf("\n  Big-O Summary:\n");
    printf("  Trie insert/search : O(L)      — L = prefix length\n");
    printf("  Hash table lookup  : O(1) avg  — chaining\n");
    printf("  Heap insert/extract: O(log n)\n");
    printf("  Graph BFS          : O(V + E)\n");
    printf("  Stack push/pop     : O(1)\n");
}

/* ============================================================
   MAIN
   ============================================================ */
int main(void) {
    HashTable ht; ht_init(&ht);
    Trie      t;  trie_init(&t);
    Stack     s;  stack_init(&s);
    Heap      h;  heap_init(&h);
    Graph     g;  graph_init(&g);

    print_banner();
    seed_demo(&ht, &t, &g, &h);

    char buf[128];
    int choice;

    while (1) {
        print_menu();
        if (!fgets(buf, sizeof(buf), stdin)) break;
        choice = atoi(buf);

        switch (choice) {

        case 1: {
            printf("  Enter user ID   : "); fgets(buf, sizeof(buf), stdin);
            int id = atoi(buf);
            printf("  Enter full name : "); fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';
            if (ht_insert(&ht, id, buf) == 0) {
                trie_insert(&t, buf, id);
                User* u = ht_search(&ht, id);
                if (u) heap_insert(&h, u);
                printf("  [✓] User '%s' (ID %d) registered.\n", buf, id);
            } else printf("  [!] ID %d already exists.\n", id);
            break;
        }

        case 2: {
            printf("  Enter prefix to search : ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';
            stack_push(&s, buf);
            int ids[10], cnt;
            trie_search(&t, buf, ids, &cnt, 10);
            if (cnt == 0) printf("  No users found matching '%s'.\n", buf);
            else {
                printf("  Found %d result(s) for '%s':\n", cnt, buf);
                for (int i = 0; i < cnt; i++) {
                    User* u = ht_search(&ht, ids[i]);
                    if (u) printf("    → [%d] %s  (friends: %d)\n", u->id, u->name, u->friend_count);
                }
            }
            break;
        }

        case 3: {
            printf("  Enter user ID A : "); fgets(buf, sizeof(buf), stdin); int a = atoi(buf);
            printf("  Enter user ID B : "); fgets(buf, sizeof(buf), stdin); int b = atoi(buf);
            User* ua = ht_search(&ht, a); User* ub = ht_search(&ht, b);
            if (!ua || !ub) { printf("  [!] User not found.\n"); break; }
            if (graph_add_friend(&g, a, b) == 0) {
                ua->friend_count++; ub->friend_count++;
                printf("  [✓] %s and %s are now friends.\n", ua->name, ub->name);
            } else printf("  [!] Already friends or limit reached.\n");
            break;
        }

        case 4: {
            printf("  Enter user ID A : "); fgets(buf, sizeof(buf), stdin); int a = atoi(buf);
            printf("  Enter user ID B : "); fgets(buf, sizeof(buf), stdin); int b = atoi(buf);
            int mutual[MAX_USERS], mc;
            graph_mutual(&g, a, b, mutual, &mc);
            if (mc == 0) printf("  No mutual friends found.\n");
            else {
                printf("  Mutual friends (%d):\n", mc);
                for (int i = 0; i < mc; i++) {
                    User* u = ht_search(&ht, mutual[i]);
                    if (u) printf("    → [%d] %s\n", u->id, u->name);
                }
            }
            break;
        }

        case 5: {
            User* top[5]; int found;
            heap_topk(&h, top, 5, &found);
            printf("  Top %d users by friend count:\n", found);
            for (int i = 0; i < found; i++)
                printf("    %d. %s — %d friends\n", i+1, top[i]->name, top[i]->friend_count);
            break;
        }

        case 6:
            printf("  Recent searches (newest first):\n");
            stack_print(&s);
            break;

        case 7:
            printf("  All registered users:\n");
            ht_print_all(&ht);
            break;

        case 8:
            run_benchmark(&t, &ht);
            break;

        case 0:
            printf("\n  Goodbye from ConnectX!\n\n");
            goto done;

        default:
            printf("  Invalid option.\n");
        }
    }

done:
    trie_free(&t);
    ht_free(&ht);
    stack_free(&s);
    return 0;
}
