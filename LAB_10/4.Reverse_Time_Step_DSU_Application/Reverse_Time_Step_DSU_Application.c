#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { long long w; int root, ver; } HE;
static HE *heap; static int hn, hcap;
static void hpush(HE e) {
    if (hn == hcap) { hcap *= 2; heap = realloc(heap, hcap * sizeof(HE)); }
    int i = hn++; heap[i] = e;
    while (i > 0) { int p = (i - 1) / 2; if (heap[p].w < heap[i].w) { HE t = heap[p]; heap[p] = heap[i]; heap[i] = t; i = p; } else break; }
}
static void hpop(void) {
    heap[0] = heap[--hn]; int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < hn && heap[l].w > heap[m].w) m = l;
        if (r < hn && heap[r].w > heap[m].w) m = r;
        if (m == i) break;
        HE t = heap[i]; heap[i] = heap[m]; heap[m] = t; i = m;
    }
}
static int find(int *p, int x) { while (p[x] != x) { p[x] = p[p[x]]; x = p[x]; } return x; }

static void reverse_dsu(int V, const long long *w, int E, const int *eu, const int *ev, const int *P, long long *ans) {
    int *start = calloc(V + 2, sizeof(int)), *adj = malloc(2 * (size_t)E * sizeof(int) + 1);
    for (int e = 0; e < E; e++) { start[eu[e] + 1]++; start[ev[e] + 1]++; }
    for (int i = 0; i < V; i++) start[i + 1] += start[i];
    int *fill = malloc((V + 1) * sizeof(int)); memcpy(fill, start, (V + 1) * sizeof(int));
    for (int e = 0; e < E; e++) { adj[fill[eu[e]]++] = ev[e]; adj[fill[ev[e]]++] = eu[e]; }
    int *parent = malloc(V * sizeof(int)), *size = calloc(V, sizeof(int)), *ver = calloc(V, sizeof(int));
    char *active = calloc(V, 1); long long *sum = malloc(V * sizeof(long long));
    hcap = 16; hn = 0; heap = malloc(hcap * sizeof(HE));
    for (int i = V - 1; i >= 0; i--) {
        long long best = 0;
        while (hn > 0) {                                    
            HE e = heap[0];
            if (parent[e.root] == e.root && ver[e.root] == e.ver) { best = e.w; break; }
            hpop();
        }
        ans[i] = best;
        int v = P[i];                                       
        active[v] = 1; parent[v] = v; size[v] = 1; sum[v] = w[v]; ver[v] = 0;
        HE e0 = {w[v], v, 0}; hpush(e0);
        for (int k = start[v]; k < start[v + 1]; k++) {
            int u = adj[k];
            if (!active[u]) continue;
            int a = find(parent, v), b = find(parent, u);
            if (a == b) continue;
            if (size[a] < size[b]) { int t = a; a = b; b = t; }
            parent[b] = a; size[a] += size[b]; sum[a] += sum[b]; ver[a]++;
            HE e = {sum[a], a, ver[a]}; hpush(e);
        }
    }
    free(start); free(adj); free(fill); free(parent); free(size); free(ver); free(active); free(sum); free(heap);
}

static unsigned long long rs = 88172645463325252ULL;
static unsigned long long rnd64(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; }
static int rnd_int(int n) { return (int)(rnd64() % (unsigned long long)n); }

static void brute(int V, const long long *w, int E, const int *eu, const int *ev, const int *P, long long *ans) {
    char del[32]; memset(del, 0, sizeof del);
    for (int i = 0; i < V; i++) {
        del[P[i]] = 1;
        int comp[32]; for (int x = 0; x < V; x++) comp[x] = x;
        for (int rep = 0; rep < V; rep++) for (int e = 0; e < E; e++)
            if (!del[eu[e]] && !del[ev[e]]) { int m = comp[eu[e]] < comp[ev[e]] ? comp[eu[e]] : comp[ev[e]]; comp[eu[e]] = comp[ev[e]] = m; }
        long long best = 0; int any = 0;
        for (int r = 0; r < V; r++) {
            long long s = 0; int cnt = 0;
            for (int x = 0; x < V; x++) if (!del[x] && comp[x] == r) { s += w[x]; cnt++; }
            if (cnt && (!any || s > best)) { best = s; any = 1; }
        }
        ans[i] = any ? best : 0;
    }
}

static int self_check(int trials) {
    int bad = 0;
    for (int t = 0; t < trials; t++) {
        int V = 1 + rnd_int(9), E = rnd_int(14);
        long long w[16], a1[16], a2[16]; int eu[16], ev[16], P[16];
        for (int i = 0; i < V; i++) { w[i] = rnd_int(14) - 4; P[i] = i; }
        for (int i = V - 1; i > 0; i--) { int j = rnd_int(i + 1); int x = P[i]; P[i] = P[j]; P[j] = x; }
        for (int e = 0; e < E; e++) { eu[e] = rnd_int(V); ev[e] = rnd_int(V); }
        reverse_dsu(V, w, E, eu, ev, P, a1); brute(V, w, E, eu, ev, P, a2);
        for (int i = 0; i < V; i++) if (a1[i] != a2[i]) { bad++; break; }
    }
    return bad;
}

static void demo(void) {
    int V = 5; long long w[] = {1, 2, 3, 4, 5};
    int eu[] = {0, 1, 2, 3}, ev[] = {1, 2, 3, 4};            
    int P[] = {2, 0, 4, 1, 3};                              
    long long ans[5];
    reverse_dsu(V, w, 4, eu, ev, P, ans);
    printf("Path 1-2-3-4-5 with weights {1,2,3,4,5}, deletion order 3,1,5,2,4:\n");
    for (int i = 0; i < V; i++) printf("  after deleting vertex %d -> max component sum = %lld\n", P[i] + 1, ans[i]);
    printf("  (expected 9, 9, 4, 4, 0)\n");
    int bad = self_check(3000);
    printf("Self-check vs brute force (3000 random graphs/orders, negative weights included): %s (%d mismatches)\n\n", bad ? "FAILED" : "PASSED", bad);
}

static void run_interactive(void) {
    int V, E;
    printf("Enter number of vertices V: "); if (scanf("%d", &V) != 1 || V <= 0) { printf("Invalid V.\n"); return; }
    long long *w = malloc(V * sizeof(long long));
    printf("Enter the %d vertex weights:\n", V);
    for (int i = 0; i < V; i++) { printf("w[%d]: ", i + 1); if (scanf("%lld", &w[i]) != 1) { free(w); return; } }
    printf("Enter number of edges E: "); if (scanf("%d", &E) != 1 || E < 0) { free(w); return; }
    int *eu = malloc((E + 1) * sizeof(int)), *ev = malloc((E + 1) * sizeof(int));
    printf("Enter the edges as: u v   (vertices 1..%d)\n", V);
    for (int i = 0; i < E; i++) {
        printf("Edge %d: ", i + 1);
        if (scanf("%d %d", &eu[i], &ev[i]) != 2 || eu[i] < 1 || eu[i] > V || ev[i] < 1 || ev[i] > V) { printf("Invalid edge.\n"); free(w); free(eu); free(ev); return; }
        eu[i]--; ev[i]--;
    }
    int *P = malloc(V * sizeof(int)); char *seen = calloc(V, 1);
    printf("Enter the deletion order (a permutation of 1..%d), %d numbers:\n", V, V);
    for (int i = 0; i < V; i++) {
        if (scanf("%d", &P[i]) != 1 || P[i] < 1 || P[i] > V || seen[P[i] - 1]) { printf("Invalid order (must be a permutation).\n"); free(w); free(eu); free(ev); free(P); free(seen); return; }
        seen[P[i] - 1] = 1; P[i]--;
    }
    long long *ans = malloc(V * sizeof(long long));
    reverse_dsu(V, w, E, eu, ev, P, ans);
    printf("\n");
    for (int i = 0; i < V; i++) printf("After deleting vertex %d: max component sum = %lld\n", P[i] + 1, ans[i]);
    free(w); free(eu); free(ev); free(P); free(seen); free(ans);
}

static void run_benchmark(void) {
    FILE *fp = fopen("Reverse_Time_Step_DSU_Application.csv", "w");
    fprintf(fp, "n,edges,time_ms\n");
    int sizes[] = {1000, 10000, 100000, 500000, 1000000, 2000000};
    for (int s = 0; s < 6; s++) {
        int V = sizes[s], E = 2 * V;
        long long *w = malloc(V * sizeof(long long)), *ans = malloc(V * sizeof(long long));
        int *eu = malloc(E * sizeof(int)), *ev = malloc(E * sizeof(int)), *P = malloc(V * sizeof(int));
        for (int i = 0; i < V; i++) { w[i] = 1 + rnd_int(1000); P[i] = i; }
        for (int i = V - 1; i > 0; i--) { int j = rnd_int(i + 1); int x = P[i]; P[i] = P[j]; P[j] = x; }
        for (int e = 0; e < E; e++) { eu[e] = rnd_int(V); ev[e] = rnd_int(V); }
        clock_t t0 = clock();
        reverse_dsu(V, w, E, eu, ev, P, ans);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("V=%8d E=%9d  time=%9.3f ms\n", V, E, ms);
        fprintf(fp, "%d,%d,%.4f\n", V, E, ms);
        free(w); free(ans); free(eu); free(ev); free(P);
    }
    fclose(fp);
}

int main(void) {
    printf("Q4: Max component sum after each vertex deletion (reverse-time DSU + lazy heap)\n");
    printf("Choose mode:\n  1 = Enter your own graph and deletion order\n  2 = Run built-in demo (+ randomized self-check)\n  3 = Run timing benchmark (writes Reverse_Time_Step_DSU_Applications.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}