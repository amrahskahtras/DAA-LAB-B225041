#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { int R, C, count; int *parent, *size; } Grid;

static Grid *grid_new(int R, int C) {
    Grid *g = malloc(sizeof(Grid));
    g->R = R; g->C = C; g->count = 0;
    g->parent = malloc((size_t)R * C * sizeof(int)); g->size = malloc((size_t)R * C * sizeof(int));
    for (long long i = 0; i < (long long)R * C; i++) { g->parent[i] = -1; g->size[i] = 0; }
    return g;
}
static void grid_free(Grid *g) { free(g->parent); free(g->size); free(g); }

static int find(int *p, int x) { while (p[x] != x) { p[x] = p[p[x]]; x = p[x]; } return x; }

static int unite(Grid *g, int a, int b) {
    a = find(g->parent, a); b = find(g->parent, b);
    if (a == b) return 0;
    if (g->size[a] < g->size[b]) { int t = a; a = b; b = t; }
    g->parent[b] = a; g->size[a] += g->size[b];
    return 1;
}

static int add_land(Grid *g, int r, int c) {
    if (r < 0 || r >= g->R || c < 0 || c >= g->C) return -1;
    int id = r * g->C + c;
    if (g->parent[id] != -1) return g->count;
    g->parent[id] = id; g->size[id] = 1; g->count++;
    static const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    for (int d = 0; d < 4; d++) {
        int nr = r + dr[d], nc = c + dc[d];
        if (nr < 0 || nr >= g->R || nc < 0 || nc >= g->C) continue;
        int nid = nr * g->C + nc;
        if (g->parent[nid] != -1 && unite(g, id, nid)) g->count--;
    }
    return g->count;
}

static unsigned long long rs = 88172645463325252ULL;
static unsigned long long rnd64(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; }
static int rnd_int(int n) { return (int)(rnd64() % (unsigned long long)n); }

/* brute force: count components of the land set by BFS */
static int brute_count(int R, int C, const char *land) {
    char seen[64] = {0}; int q[64], cnt = 0;
    for (int s = 0; s < R * C; s++) {
        if (!land[s] || seen[s]) continue;
        cnt++; int h = 0, t = 0; q[t++] = s; seen[s] = 1;
        while (h < t) {
            int x = q[h++], r = x / C, c = x % C;
            int nb[4][2] = {{r-1,c},{r+1,c},{r,c-1},{r,c+1}};
            for (int d = 0; d < 4; d++) {
                int nr = nb[d][0], nc = nb[d][1];
                if (nr < 0 || nr >= R || nc < 0 || nc >= C) continue;
                int y = nr * C + nc;
                if (land[y] && !seen[y]) { seen[y] = 1; q[t++] = y; }
            }
        }
    }
    return cnt;
}

static int self_check(int trials) {
    int bad = 0;
    for (int t = 0; t < trials; t++) {
        int R = 1 + rnd_int(7), C = 1 + rnd_int(7), k = 1 + rnd_int(40);
        Grid *g = grid_new(R, C); char land[64] = {0};
        for (int i = 0; i < k; i++) {
            int r = rnd_int(R), c = rnd_int(C);
            land[r * C + c] = 1;
            if (add_land(g, r, c) != brute_count(R, C, land)) bad++;
        }
        grid_free(g);
    }
    return bad;
}

static void print_grid(Grid *g) {
    for (int r = 0; r < g->R; r++) { printf("    "); for (int c = 0; c < g->C; c++) putchar(g->parent[r * g->C + c] != -1 ? '#' : '.'); printf("\n"); }
}

static void demo(void) {
    int pos[][2] = {{0,0},{0,1},{1,2},{2,1},{1,1},{1,1}};
    Grid *g = grid_new(3, 3);
    printf("3x3 grid, land added at (0,0) (0,1) (1,2) (2,1) (1,1) and (1,1) again:\n");
    for (int i = 0; i < 6; i++) printf("  add (%d,%d) -> islands = %d\n", pos[i][0], pos[i][1], add_land(g, pos[i][0], pos[i][1]));
    printf("  final grid:\n"); print_grid(g);
    grid_free(g);
    int bad = self_check(3000);
    printf("Self-check vs brute-force BFS on 3000 random grids/operation sequences: %s (%d mismatches)\n\n", bad ? "FAILED" : "PASSED", bad);
}

static void run_interactive(void) {
    int R, C, k;
    printf("Enter grid rows R and columns C: "); if (scanf("%d %d", &R, &C) != 2 || R <= 0 || C <= 0 || (long long)R * C > 100000000LL) { printf("Invalid size.\n"); return; }
    printf("Enter number of land operations k: "); if (scanf("%d", &k) != 1 || k < 0) { printf("Invalid k.\n"); return; }
    Grid *g = grid_new(R, C);
    printf("Enter each land position as: row col   (0-indexed)\n");
    for (int i = 0; i < k; i++) {
        int r, c; printf("Op %d: ", i + 1);
        if (scanf("%d %d", &r, &c) != 2) { grid_free(g); return; }
        int res = add_land(g, r, c);
        if (res < 0) printf("  -> (%d,%d) is outside the grid, ignored\n", r, c); else printf("  -> islands = %d\n", res);
    }
    if ((long long)R * C <= 400) { printf("Final grid (# = land):\n"); print_grid(g); }
    grid_free(g);
}

static void run_benchmark(void) {
    FILE *fp = fopen("Dynamic_Connected_Components_in_a_Grid.csv", "w");
    fprintf(fp, "k,cells,time_ms\n");
    int ks[] = {1000, 10000, 100000, 500000, 1000000, 2000000, 4000000};
    for (int s = 0; s < 7; s++) {
        int k = ks[s], side = 1; while ((long long)side * side < 2LL * k) side++;   /* grid ~2k cells: fixed density */
        Grid *g = grid_new(side, side);
        clock_t t0 = clock();
        for (int i = 0; i < k; i++) add_land(g, rnd_int(side), rnd_int(side));
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("k=%8d grid=%dx%d  time=%9.3f ms  islands=%d\n", k, side, side, ms, g->count);
        fprintf(fp, "%d,%lld,%.4f\n", k, (long long)side * side, ms);
        grid_free(g);
    }
    fclose(fp);
}

int main(void) {
    printf("Q2: Number of Islands II (DSU with coordinate flattening, O(alpha) per operation)\n");
    printf("Choose mode:\n  1 = Enter your own grid and land positions\n  2 = Run built-in demo (+ randomized self-check)\n  3 = Run timing benchmark (writes Dynamic_Connected_Components_in_a_Grid.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}