#ifndef GRAPH_H
#define GRAPH_H

#define MAX_USERS   500
#define MAX_FRIENDS  50

// Adjacency list graph for friend connections
typedef struct {
    int adj[MAX_USERS][MAX_FRIENDS];  // adj[user_id] = list of friend IDs
    int degree[MAX_USERS];            // number of friends per user
    int user_count;
} Graph;

void graph_init(Graph* g);
int  graph_add_friend(Graph* g, int user_a, int user_b);
void graph_bfs_mutual(Graph* g, int user_a, int user_b,
                      int* mutual, int* mutual_count);
void graph_print_friends(Graph* g, int user_id);

#endif
