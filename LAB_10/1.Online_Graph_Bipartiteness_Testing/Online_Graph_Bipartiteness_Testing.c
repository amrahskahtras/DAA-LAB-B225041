#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { int n; int *parent, *size; unsigned char *par; } BDSU;

static BDSU *bd_new(int n) {
    BDSU *d = malloc(sizeof(BDSU));
    d->n = n; d->parent = malloc(n * sizeof(int)); d->size = malloc(n * sizeof(int)); d->par = calloc(n, 1);
    for (int i = 0; i < n; i++) { d->parent[i] = i; d->size[i] = 1; }
    return d;
}
static void bd_free(BDSU *d) { free(d->parent); free(d->size); free(d->par); free(d); }

static int bd_find(BDSU *d, int x) {
    int p = d->parent[x];
    if (p == x) return x;
    int r = bd_find(d, p);
    d->par[x] ^= d->par[p];
    d->parent[x] = r;
    return r;
}

static int bd_add_edge(BDSU *d, int u, int v) {
    int ru = bd_find(d, u), rv = bd_find(d, v);
    if (ru == rv) return d->par[u] != d->par[v];
    int pu = d->par[u], pv = d->par[v];
    if (d->size[ru] < d->size[rv]) { int t = ru; ru = rv; rv = t; }
    d->parent[rv] = ru; d->par[rv] = pu ^ pv ^ 1; d->size[ru] += d->size[rv];
    return 1;
}
static int bd_color(BDSU *d, int x) { bd_find(d, x); return d->par[x]; }

static unsigned long long rs = 88172645463325252ULL;
static unsigned long long rnd64(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; }
static int rnd_int(int n) { return (int)(rnd64() % (unsigned long long)n); }

static int brute_bipartite(int n, int adj[16][16]) {
    int col[16], q[16];
    for (int i = 0; i < n; i++) col[i] = -1;
    for (int s = 0; s < n; s++) {
        if (col[s] != -1) continue;
        int h = 0, t = 0; q[t++] = s; col[s] = 0;
        while (h < t) {
            int x = q[h++];
            for (int y = 0; y < n; y++) if (adj[x][y]) {
                if (col[y] == -1) { col[y] = col[x] ^ 1; q[t++] = y; }
                else if (col[y] == col[x]) return 0;
            }
        }
    }
    return 1;
}

static int self_check(int trials) {
    int bad = 0;
    for (int t = 0; t < trials; t++) {
        int V = 1 + rnd_int(10), E = rnd_int(25);
        BDSU *d = bd_new(V);
        int adj[16][16]; memset(adj, 0, sizeof adj);
        for (int e = 0; e < E; e++) {
            int u = rnd_int(V), v = rnd_int(V);
            int got = bd_add_edge(d, u, v);
            int tent[16][16]; memcpy(tent, adj, sizeof adj); tent[u][v] = tent[v][u] = 1;
            int expect = brute_bipartite(V, tent);         
            if (u == v) expect = 0;
            if (got != expect) bad++;
            if (expect) adj[u][v] = adj[v][u] = 1;
        }
        for (int x = 0; x < V; x++) for (int y = 0; y < V; y++)  
            if (adj[x][y] && bd_color(d, x) == bd_color(d, y)) bad++;
        bd_free(d);
    }
    return bad;
}

static void demo(void) {
    int V = 6, E = 9;
    int ed[][2] = {{1,2},{2,3},{3,4},{4,1},{1,3},{5,6},{6,6},{4,5},{6,1}};
    BDSU *d = bd_new(V);
    printf("V = %d, edges arrive online:\n", V);
    for (int i = 0; i < E; i++) {
        int ok = bd_add_edge(d, ed[i][0] - 1, ed[i][1] - 1);
        printf("  add (%d,%d): %s\n", ed[i][0], ed[i][1], ok ? "ACCEPTED" : "REJECTED (would create an odd cycle)");
    }
    printf("Final 2-colouring of the accepted graph: ");
    for (int v = 0; v < V; v++) printf("%d:%c ", v + 1, bd_color(d, v) ? 'B' : 'A');
    printf("\n");
    bd_free(d);
    int bad = self_check(3000);
    printf("Self-check vs brute-force BFS 2-colouring on 3000 random edge sequences: %s (%d mismatches)\n\n", bad ? "FAILED" : "PASSED", bad);
}

static void run_interactive(void) {
    int V, m;
    printf("Enter number of vertices V: "); if (scanf("%d", &V) != 1 || V <= 0) { printf("Invalid V.\n"); return; }
    printf("Enter number of edges m: "); if (scanf("%d", &m) != 1 || m < 0) { printf("Invalid m.\n"); return; }
    BDSU *d = bd_new(V);
    printf("Enter the edges one by one as: u v   (vertices numbered 1..%d)\n", V);
    for (int i = 0; i < m; i++) {
        int u, v; printf("Edge %d: ", i + 1);
        if (scanf("%d %d", &u, &v) != 2 || u < 1 || u > V || v < 1 || v > V) { printf("Invalid edge.\n"); bd_free(d); return; }
        printf("  -> %s\n", bd_add_edge(d, u - 1, v - 1) ? "ACCEPTED" : "REJECTED (would create an odd cycle)");
    }
    printf("\nColouring of the accepted (bipartite) graph:\n  Side A:"); for (int v = 0; v < V; v++) if (!bd_color(d, v)) printf(" %d", v + 1);
    printf("\n  Side B:"); for (int v = 0; v < V; v++) if (bd_color(d, v)) printf(" %d", v + 1);
    printf("\n");
    bd_free(d);
}

static void run_benchmark(void) {
    FILE *fp = fopen("Online_Graph_Bipartiteness_Testing.csv", "w");
    fprintf(fp, "n,edges,time_ms,accepted,rejected\n");
    int sizes[] = {1000, 10000, 100000, 500000, 1000000, 2000000, 4000000};
    for (int s = 0; s < 7; s++) {
        int V = sizes[s]; long long E = 2LL * V;
        BDSU *d = bd_new(V); long long acc = 0;
        clock_t t0 = clock();
        for (long long e = 0; e < E; e++) acc += bd_add_edge(d, rnd_int(V), rnd_int(V));
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("V=%8d E=%9lld  time=%9.3f ms  accepted=%lld rejected=%lld\n", V, E, ms, acc, E - acc);
        fprintf(fp, "%d,%lld,%.4f,%lld,%lld\n", V, E, ms, acc, E - acc);
        bd_free(d);
    }
    fclose(fp);
}

int main(void) {
    printf("Q1: Online bipartiteness testing (DSU with parity, O(alpha(V)) per edge)\n");
    printf("Choose mode:\n  1 = Enter your own graph / edge stream\n  2 = Run built-in demo (+ randomized self-check)\n  3 = Run timing benchmark (writes Online_Graph_Bipartiteness_Testing.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}