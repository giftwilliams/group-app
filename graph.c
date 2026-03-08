#include <stdio.h>
#include <string.h>
#include "graph.h"

void graph_init(Graph* g) {
    memset(g->adj,    0, sizeof(g->adj));
    memset(g->degree, 0, sizeof(g->degree));
    g->user_count = 0;
}

// Add a bidirectional friendship — O(1)
int graph_add_friend(Graph* g, int user_a, int user_b) {
    if (user_a < 0 || user_a >= MAX_USERS) return -1;
    if (user_b < 0 || user_b >= MAX_USERS) return -1;
    if (g->degree[user_a] >= MAX_FRIENDS)  return -1;
    if (g->degree[user_b] >= MAX_FRIENDS)  return -1;

    // Check if already friends
    for (int i = 0; i < g->degree[user_a]; i++)
        if (g->adj[user_a][i] == user_b) return -1;

    g->adj[user_a][g->degree[user_a]++] = user_b;
    g->adj[user_b][g->degree[user_b]++] = user_a;
    return 0;
}

// BFS to find mutual friends between user_a and user_b — O(V + E)
void graph_bfs_mutual(Graph* g, int user_a, int user_b,
                      int* mutual, int* mutual_count) {
    *mutual_count = 0;

    // Mark friends of user_a
    int visited[MAX_USERS] = {0};
    for (int i = 0; i < g->degree[user_a]; i++)
        visited[g->adj[user_a][i]] = 1;

    // Check friends of user_b against user_a's friends
    for (int i = 0; i < g->degree[user_b]; i++) {
        int friend_id = g->adj[user_b][i];
        if (visited[friend_id] && friend_id != user_a) {
            mutual[(*mutual_count)++] = friend_id;
        }
    }
}

void graph_print_friends(Graph* g, int user_id) {
    if (user_id < 0 || user_id >= MAX_USERS) return;
    printf("  Friends of user %d: ", user_id);
    if (g->degree[user_id] == 0) { printf("(none)\n"); return; }
    for (int i = 0; i < g->degree[user_id]; i++)
        printf("%d ", g->adj[user_id][i]);
    printf("\n");
}
