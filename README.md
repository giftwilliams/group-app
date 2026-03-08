# ConnectX — Scalable Friend Search & Autocomplete
### DSA-CH23 | Group Project | Variant A5
---

## Problem Statement
ConnectX simulates the friend-search and autocomplete feature of a social network (inspired by Facebook/Instagram). A user types a name prefix and instantly gets ranked suggestions. The system also supports friend connections, mutual-friend discovery, and session-based search history.

---

## Architecture Diagram

```
 ┌──────────────────────────────────────────────────────┐
 │                    ConnectX Core                     │
 │                                                      │
 │  ┌────────┐   ┌────────────┐   ┌──────────────────┐  │
 │  │  Trie  │   │ Hash Table │   │  Friend Graph    │  │
 │  │(search)│   │ (profiles) │   │ (adjacency list) │  │
 │  └───┬────┘   └─────┬──────┘   └────────┬─────────┘  │
 │      │              │                   │             │
 │  ┌───▼──────────────▼───────────────────▼──────────┐  │
 │  │          Result Layer                           │  │
 │  │   Max-Heap (top-k)  +  Stack (history)         │  │
 │  └─────────────────────────────────────────────────┘  │
 └──────────────────────────────────────────────────────┘
```

---

## Features
- Prefix-based autocomplete using a **Trie**
- User profile lookup in O(1) via **Hash Table** (chaining)
- Top-5 ranked results by friend count using a **Max-Heap**
- Mutual friend discovery via **BFS on a Graph**
- Session search history (last 5) using a **Stack**
- Built-in benchmark with Big-O analysis

---

## System Design — Chapter 23 Five-Step Process

### Step 1: Use Cases
1. Register a new user (name + ID)
2. Search users by name prefix → autocomplete
3. Add a friendship between two users
4. Find mutual friends between two users
5. View top-5 most-connected users
6. View recent search history (last 5 queries)

### Step 2: Constraints & Analysis
| Parameter | Value |
|---|---|
| Max users | 500 |
| Max username length | 50 chars |
| Max friends per user | 50 |
| Autocomplete results | Top 5–10 |
| History depth | 5 entries |
| Read/Write ratio | ~80% reads |

### Step 3: Basic Design
- **Trie** for O(L) prefix search (L = prefix length)
- **Hash Table** (array of linked lists, size 512) for O(1) avg user lookup
- **Adjacency List Graph** for O(1) friend add, O(V+E) BFS
- **Max-Heap** for O(log n) insert, O(k log n) top-k retrieval
- **Stack** (linked list) for O(1) push/pop search history

### Step 4: Bottlenecks
| Bottleneck | Solution |
|---|---|
| Trie memory on large names | Only allocate children that exist (sparse) |
| Too many autocomplete results | Heap limits output to top-k |
| Graph BFS too deep | Limit traversal to mutual friends (depth 1) |
| Hash collisions | Chaining with linked lists |

### Step 5: Scalability
| Scaling strategy | Implementation |
|---|---|
| >500 users | Hash-partition trie by first letter (a–z shards) |
| Multiple servers | Consistent hashing on user ID |
| Hot-user caching | LRU cache layer before hash table |
| Read-heavy workload | Benchmark shows hash lookup at ~0ms for 10,000 ops |

---

## Data Structures & Algorithms Used

| DS / Algorithm | File | Purpose | Big-O |
|---|---|---|---|
| Trie | trie.c | Autocomplete prefix search | O(L) insert/search |
| Hash Table | hashtable.c | User profile lookup | O(1) avg |
| Max-Heap | heap.c | Top-k by friend count | O(log n) insert |
| Graph (adj list) | graph.c | Friend connections + BFS | O(V+E) BFS |
| Stack | stack.c | Search history (last 5) | O(1) push/pop |

---

## How to Run

### Requirements
- GCC (any version supporting C99+)

### Compile
```bash
gcc -Wall -o connectx main.c trie.c hashtable.c stack.c heap.c graph.c
```

### Run
```bash
./connectx
```

### Sample Interaction
```
Choice: 2
Enter prefix to search: al
Found 1 result(s) for 'al':
  → [0] Alice Johnson  (friends: 4)

Choice: 4
Enter user ID A: 0
Enter user ID B: 1
Mutual friends (2):
  → [2] Charlie Brown
  → [3] Diana Prince
```

---

## File Structure
```
ConnectX/
├── main.c          ← Entry point, menu, demo seed, benchmark
├── trie.h / .c     ← Trie autocomplete engine
├── hashtable.h/.c  ← Hash table user profiles
├── heap.h / .c     ← Max-heap for top-k ranking
├── stack.h / .c    ← Stack for search history
├── graph.h / .c    ← Adjacency list graph + BFS
└── README.md
```

---

## Team Member Roles
| Member | Role |
|---|---|
| [Your Name] | Trie + Autocomplete (A5 lead) |
| Member 2 | Hash Table + User registration |
| Member 3 | Graph + BFS mutual friends |
| Member 4 | Heap + Top-k ranking |
| Member 5 | Stack + Search history |
| Member 6 | Main menu + integration |
| Member 7 | Testing + test cases |
| Member 8 | Benchmark + complexity analysis |
| Member 9 | README + documentation |
| Member 10 | Demo video + presentation |

---

## Complexity Benchmark (built-in)
Run option `8` from the menu to see live benchmark results:
- Trie search ×10,000 operations
- Hash table lookup ×10,000 operations
- Full Big-O table printed to console
