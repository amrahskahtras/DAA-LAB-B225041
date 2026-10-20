#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define NONE LLONG_MIN

typedef struct { long long w; int root, ver; } HE;
typedef struct {
    int V, K; int *parent, *size, *ver; long long *wsum;
    HE *heap; int hn, hcap;
} Ctx;

static void hpush(Ctx *c, HE e) {
    if (c->hn == c->hcap) { c->hcap *= 2; c->heap = realloc(c->heap, c->hcap * sizeof(HE)); }
    int i = c->hn++; c->heap[i] = e;
    while (i > 0) { int p = (i - 1) / 2; if (c->heap[p].w < c->heap[i].w) { HE t = c->heap[p]; c->heap[p] = c->heap[i]; c->heap[i] = t; i = p; } else break; }
}
static void hpop(Ctx *c) {
    c->heap[0] = c->heap[--c->hn]; int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < c->hn && c->heap[l].w > c->heap[m].w) m = l;
        if (r < c->hn && c->heap[r].w > c->heap[m].w) m = r;
        if (m == i) break;
        HE t = c->heap[i]; c->heap[i] = c->heap[m]; c->heap[m] = t; i = m;
    }
}

static int find(int *p, int x) { while (p[x] != x) { p[x] = p[p[x]]; x = p[x]; } return x; }

static Ctx *ctx_new(int V, int K, const long long *w) {
    Ctx *c = malloc(sizeof(Ctx));
    c->V = V; c->K = K; c->parent = malloc(V * sizeof(int)); c->size = malloc(V * sizeof(int));
    c->ver = calloc(V, sizeof(int)); c->wsum = malloc(V * sizeof(long long));
    c->hcap = 16; c->hn = 0; c->heap = malloc(c->hcap * sizeof(HE));
    for (int i = 0; i < V; i++) {
        c->parent[i] = i; c->size[i] = 1; c->wsum[i] = w[i];
        if (1 <= K) { HE e = {w[i], i, 0}; hpush(c, e); }
    }
    return c;
}
static void ctx_free(Ctx *c) { free(c->parent); free(c->size); free(c->ver); free(c->wsum); free(c->heap); free(c); }

static long long query(Ctx *c) {
    while (c->hn > 0) {
        HE e = c->heap[0];
        if (c->parent[e.root] == e.root && c->ver[e.root] == e.ver && c->size[e.root] <= c->K) return e.w;
        hpop(c);
    }
    return NONE;
}

static long long add_edge(Ctx *c, int u, int v) {
    int a = find(c->parent, u), b = find(c->parent, v);
    if (a != b) {
        if (c->size[a] < c->size[b]) { int t = a; a = b; b = t; }
        c->parent[b] = a; c->size[a] += c->size[b]; c->wsum[a] += c->wsum[b]; c->ver[a]++;
        if (c->size[a] <= c->K) { HE e = {c->wsum[a], a, c->ver[a]}; hpush(c, e); }
    }
    return query(c);
}

static unsigned long long rs = 88172645463325252ULL;
static unsigned long long rnd64(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; }
static int rnd_int(int n) { return (int)(rnd64() % (unsigned long long)n); }

/* brute force: rebuild components from scratch after each edge */
static long long brute(int V, int K, const long long *w, int (*ed)[2], int upto) {
    int comp[32]; for (int i = 0; i < V; i++) comp[i] = i;
    for (int e = 0; e <= upto; e++) {
        int a = comp[ed[e][0]], b = comp[ed[e][1]];
        if (a != b) for (int i = 0; i < V; i++) if (comp[i] == b) comp[i] = a;
    }
    long long best = NONE;
    for (int r = 0; r < V; r++) {
        int sz = 0; long long s = 0;
        for (int i = 0; i < V; i++) if (comp[i] == r) { sz++; s += w[i]; }
        if (sz > 0 && sz <= K && (best == NONE || s > best)) best = s;
    }
    return best;
}

static int self_check(int trials) {
    int bad = 0;
    for (int t = 0; t < trials; t++) {
        int V = 1 + rnd_int(10), K = 1 + rnd_int(V + 2), E = 1 + rnd_int(18);
        long long w[16]; int ed[32][2];
        for (int i = 0; i < V; i++) w[i] = rnd_int(16) - 5;             
        Ctx *c = ctx_new(V, K, w);
        for (int e = 0; e < E; e++) {
            ed[e][0] = rnd_int(V); ed[e][1] = rnd_int(V);
            if (add_edge(c, ed[e][0], ed[e][1]) != brute(V, K, w, ed, e)) bad++;
        }
        ctx_free(c);
    }
    return bad;
}

static void print_ans(long long a) { if (a == NONE) printf("none (no component has size <= K)"); else printf("%lld", a); }

static void demo(void) {
    int V = 5, K = 3; long long w[] = {4, 1, 7, 2, 5};
    int ed[][3] = {{1,2,10},{2,3,20},{4,5,5},{1,4,8}};
    Ctx *c = ctx_new(V, K, w);
    printf("V=5, K=3, vertex weights = {4,1,7,2,5}\n  initial best = "); print_ans(query(c)); printf("\n");
    for (int i = 0; i < 4; i++) {
        printf("  add edge (%d,%d, w=%d) -> best weight of a component with size <= %d = ", ed[i][0], ed[i][1], ed[i][2], K);
        print_ans(add_edge(c, ed[i][0] - 1, ed[i][1] - 1)); printf("\n");
    }
    ctx_free(c);
    int bad = self_check(3000);
    printf("Self-check vs brute force (3000 random cases, negative weights included): %s (%d mismatches)\n\n", bad ? "FAILED" : "PASSED", bad);
}

static void run_interactive(void) {
    int V, K, m;
    printf("Enter number of vertices V: "); if (scanf("%d", &V) != 1 || V <= 0) { printf("Invalid V.\n"); return; }
    printf("Enter size threshold K: "); if (scanf("%d", &K) != 1) { printf("Invalid K.\n"); return; }
    long long *w = malloc(V * sizeof(long long));
    printf("Enter the %d vertex weights:\n", V);
    for (int i = 0; i < V; i++) { printf("w[%d]: ", i + 1); if (scanf("%lld", &w[i]) != 1) { free(w); return; } }
    printf("Enter number of edges m: "); if (scanf("%d", &m) != 1 || m < 0) { free(w); return; }
    Ctx *c = ctx_new(V, K, w);
    printf("initial best = "); print_ans(query(c)); printf("\n");
    printf("Enter edges one by one as: u v edge_weight   (vertices 1..%d)\n", V);
    for (int i = 0; i < m; i++) {
        int u, v; long long ew; printf("Edge %d: ", i + 1);
        if (scanf("%d %d %lld", &u, &v, &ew) != 3 || u < 1 || u > V || v < 1 || v > V) { printf("Invalid edge.\n"); break; }
        printf("  -> best = "); print_ans(add_edge(c, u - 1, v - 1)); printf("\n");
    }
    ctx_free(c); free(w);
}

static void run_benchmark(void) {
    FILE *fp = fopen("Maximum_Weight_Component_with_Size_Constraints.csv", "w");
    fprintf(fp, "n,edges,time_ms\n");
    int sizes[] = {1000, 10000, 100000, 500000, 1000000, 2000000};
    for (int s = 0; s < 6; s++) {
        int V = sizes[s]; long long E = 2LL * V;
        long long *w = malloc(V * sizeof(long long));
        for (int i = 0; i < V; i++) w[i] = rnd_int(1100) - 100;
        clock_t t0 = clock();
        Ctx *c = ctx_new(V, 1000, w);
        long long sink = 0;
        for (long long e = 0; e < E; e++) sink += add_edge(c, rnd_int(V), rnd_int(V)) & 1;
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("V=%8d E=%9lld  time=%9.3f ms (checksum %lld)\n", V, E, ms, sink);
        fprintf(fp, "%d,%lld,%.4f\n", V, E, ms);
        ctx_free(c); free(w);
    }
    fclose(fp);
}

int main(void) {
    printf("Q3: Max-weight component with size <= K under edge insertions (DSU + lazy max-heap)\n");
    printf("Choose mode:\n  1 = Enter your own graph / edge stream\n  2 = Run built-in demo (+ randomized self-check)\n  3 = Run timing benchmark (writes Maximum_Weight_Component_with_Size_Constraints.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}