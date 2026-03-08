#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashtable.h"
#include "trie.h"
#include "stack.h"
#include "heap.h"
#include "graph.h"

/* ─── helpers ─────────────────────────────────────────────────────────── */

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

/* ─── seed 20 demo users ───────────────────────────────────────────────── */

static void seed_demo_data(HashTable* ht, Trie* t, Graph* g, Heap* h) {
    // {id, name, friend_count_offset}
    const char* names[] = {
        "Alice Johnson",  "Bob Marley",    "Charlie Brown",  "Diana Prince",
        "Edward Norton",  "Fatima Hassan",  "George Clooney", "Hannah White",
        "Ivan Petrov",    "Julia Roberts",  "Kevin Hart",     "Laura Croft",
        "Mohammed Ali",   "Nina Simone",    "Oscar Wilde",    "Paula Abdul",
        "Quinn Hughes",   "Rachel Green",   "Samuel Adams",   "Tina Turner"
    };
    int n = 20;

    for (int i = 0; i < n; i++) {
        ht_insert(ht, i, names[i]);
        trie_insert(t, names[i], i);
        User* u = ht_search(ht, i);
        if (u) heap_insert(h, u);
    }

    // Add some friendships
    int pairs[][2] = {
        {0,1},{0,2},{0,5},{1,2},{1,3},{2,4},{3,4},{3,5},
        {5,6},{6,7},{7,8},{8,9},{9,10},{10,11},{11,12},
        {0,3},{4,6},{7,9},{12,13},{14,15}
    };
    int np = 20;
    for (int i = 0; i < np; i++) {
        int a = pairs[i][0], b = pairs[i][1];
        if (graph_add_friend(g, a, b) == 0) {
            User* ua = ht_search(ht, a);
            User* ub = ht_search(ht, b);
            if (ua) ua->friend_count++;
            if (ub) ub->friend_count++;
        }
    }

    printf("  [✓] Loaded %d demo users with sample friendships.\n", n);
}

/* ─── benchmark ────────────────────────────────────────────────────────── */

static void run_benchmark(Trie* t, HashTable* ht) {
    printf("\n  --- Benchmark ---\n");

    // Trie prefix search x 10000
    int ids[10], cnt;
    clock_t start = clock();
    for (int i = 0; i < 10000; i++)
        trie_search(t, "a", ids, &cnt, 10);
    clock_t end = clock();
    printf("  Trie search x10,000  : %.3f ms\n",
           1000.0 * (end - start) / CLOCKS_PER_SEC);

    // Hash table lookup x 10000
    start = clock();
    for (int i = 0; i < 10000; i++)
        ht_search(ht, i % 20);
    end = clock();
    printf("  Hash lookup  x10,000 : %.3f ms\n",
           1000.0 * (end - start) / CLOCKS_PER_SEC);

    printf("\n  Big-O Summary:\n");
    printf("  %-30s O(L)       L = prefix length\n", "Trie insert/search:");
    printf("  %-30s O(1) avg   chaining\n",           "Hash table lookup:");
    printf("  %-30s O(log n)              \n",          "Heap insert/extract:");
    printf("  %-30s O(V + E)              \n",          "Graph BFS:");
    printf("  %-30s O(1) amortized        \n",          "Stack push/pop:");
}

/* ─── main ─────────────────────────────────────────────────────────────── */

int main(void) {
    HashTable ht;  ht_init(&ht);
    Trie      t;   trie_init(&t);
    Stack     s;   stack_init(&s);
    Heap      h;   heap_init(&h);
    Graph     g;   graph_init(&g);

    print_banner();
    seed_demo_data(&ht, &t, &g, &h);

    int choice;
    char buf[128];

    while (1) {
        print_menu();
        if (!fgets(buf, sizeof(buf), stdin)) break;
        choice = atoi(buf);

        switch (choice) {

        /* ── 1. Register user ── */
        case 1: {
            printf("  Enter user ID   : ");
            fgets(buf, sizeof(buf), stdin);
            int id = atoi(buf);

            printf("  Enter full name : ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';

            if (ht_insert(&ht, id, buf) == 0) {
                trie_insert(&t, buf, id);
                User* u = ht_search(&ht, id);
                if (u) heap_insert(&h, u);
                printf("  [✓] User '%s' (ID %d) registered.\n", buf, id);
            } else {
                printf("  [!] ID %d already exists.\n", id);
            }
            break;
        }

        /* ── 2. Autocomplete search ── */
        case 2: {
            printf("  Enter prefix to search : ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';

            stack_push(&s, buf);   // save to history

            int result_ids[10], cnt;
            trie_search(&t, buf, result_ids, &cnt, 10);

            if (cnt == 0) {
                printf("  No users found matching '%s'.\n", buf);
            } else {
                printf("  Found %d result(s) for '%s':\n", cnt, buf);
                for (int i = 0; i < cnt; i++) {
                    User* u = ht_search(&ht, result_ids[i]);
                    if (u) printf("    → [%d] %s  (friends: %d)\n",
                                  u->id, u->name, u->friend_count);
                }
            }
            break;
        }

        /* ── 3. Add friendship ── */
        case 3: {
            printf("  Enter user ID A : ");
            fgets(buf, sizeof(buf), stdin);
            int a = atoi(buf);

            printf("  Enter user ID B : ");
            fgets(buf, sizeof(buf), stdin);
            int b = atoi(buf);

            User* ua = ht_search(&ht, a);
            User* ub = ht_search(&ht, b);
            if (!ua || !ub) { printf("  [!] One or both users not found.\n"); break; }

            if (graph_add_friend(&g, a, b) == 0) {
                ua->friend_count++;
                ub->friend_count++;
                printf("  [✓] %s and %s are now friends.\n", ua->name, ub->name);
            } else {
                printf("  [!] Already friends or limit reached.\n");
            }
            break;
        }

        /* ── 4. Mutual friends ── */
        case 4: {
            printf("  Enter user ID A : ");
            fgets(buf, sizeof(buf), stdin);
            int a = atoi(buf);

            printf("  Enter user ID B : ");
            fgets(buf, sizeof(buf), stdin);
            int b = atoi(buf);

            int mutual[MAX_USERS], mc;
            graph_bfs_mutual(&g, a, b, mutual, &mc);

            if (mc == 0) {
                printf("  No mutual friends found.\n");
            } else {
                printf("  Mutual friends (%d):\n", mc);
                for (int i = 0; i < mc; i++) {
                    User* u = ht_search(&ht, mutual[i]);
                    if (u) printf("    → [%d] %s\n", u->id, u->name);
                }
            }
            break;
        }

        /* ── 5. Top-5 by friend count ── */
        case 5: {
            User* top[5];
            int found;
            heap_get_topk(&h, top, 5, &found);
            printf("  Top %d users by friend count:\n", found);
            for (int i = 0; i < found; i++)
                printf("    %d. %s — %d friends\n",
                       i + 1, top[i]->name, top[i]->friend_count);
            break;
        }

        /* ── 6. Search history ── */
        case 6:
            printf("  Recent searches (newest first):\n");
            stack_print_history(&s);
            break;

        /* ── 7. All users ── */
        case 7:
            printf("  All registered users:\n");
            ht_print_all(&ht);
            break;

        /* ── 8. Benchmark ── */
        case 8:
            run_benchmark(&t, &ht);
            break;

        /* ── 0. Exit ── */
        case 0:
            printf("\n  Goodbye from ConnectX!\n\n");
            goto done;

        default:
            printf("  Invalid option. Try again.\n");
        }
    }

done:
    trie_free(&t);
    ht_free(&ht);
    stack_free(&s);
    return 0;
}
