#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int find(int *p, int x) { while (p[x] != x) { p[x] = p[p[x]]; x = p[x]; } return x; }
static int cmp_int(const void *a, const void *b) { int x = *(const int*)a, y = *(const int*)b; return (x > y) - (x < y); }

static void smallest_perm(int n, const int *A, int m, const int *eu, const int *ev, int by_value, int *out) {
    int *parent = malloc(n * sizeof(int)), *pos = NULL;
    for (int i = 0; i < n; i++) parent[i] = i;
    if (by_value) { pos = malloc((n + 1) * sizeof(int)); for (int i = 0; i < n; i++) pos[A[i]] = i; }
    for (int e = 0; e < m; e++) {
        int a = by_value ? pos[eu[e]] : eu[e], b = by_value ? pos[ev[e]] : ev[e];
        a = find(parent, a); b = find(parent, b);
        if (a != b) parent[a] = b;
    }
    int *root = malloc(n * sizeof(int)), *cnt = calloc(n + 1, sizeof(int));
    for (int i = 0; i < n; i++) { root[i] = find(parent, i); cnt[root[i] + 1]++; }
    for (int r = 0; r < n; r++) cnt[r + 1] += cnt[r];              
    int *fill = malloc(n * sizeof(int)), *gpos = malloc(n * sizeof(int)), *gval = malloc(n * sizeof(int));
    memcpy(fill, cnt, n * sizeof(int));
    for (int i = 0; i < n; i++) { int k = fill[root[i]]++; gpos[k] = i; gval[k] = A[i]; }
    for (int r = 0; r < n; r++) {
        int s = cnt[r], len = cnt[r + 1] - cnt[r];
        if (len == 0) continue;
        qsort(gval + s, len, sizeof(int), cmp_int);
        for (int j = 0; j < len; j++) out[gpos[s + j]] = gval[s + j];
    }
    free(parent); free(pos); free(root); free(cnt); free(fill); free(gpos); free(gval);
}

static unsigned long long rs = 88172645463325252ULL;
static unsigned long long rnd64(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; }
static int rnd_int(int n) { return (int)(rnd64() % (unsigned long long)n); }

static void brute(int n, const int *A, int m, const int *eu, const int *ev, int by_value, int *out) {
    static char seen[2097152]; int total = 1; for (int i = 0; i < n; i++) total *= 8;
    int *queue = malloc(total * sizeof(int)); int h = 0, t = 0;
    int enc = 0; for (int i = 0; i < n; i++) enc = enc * 8 + A[i];
    memset(seen, 0, total); seen[enc] = 1; queue[t++] = enc; int best = enc;
    while (h < t) {
        int cur = queue[h++], arr[8], x = cur;
        for (int i = n - 1; i >= 0; i--) { arr[i] = x % 8; x /= 8; }
        if (cur < best) best = cur;
        for (int e = 0; e < m; e++) {
            int a = eu[e], b = ev[e];
            if (by_value) { int pa = -1, pb = -1; for (int i = 0; i < n; i++) { if (arr[i] == a) pa = i; if (arr[i] == b) pb = i; } a = pa; b = pb; }
            int tmp = arr[a]; arr[a] = arr[b]; arr[b] = tmp;
            int ne = 0; for (int i = 0; i < n; i++) ne = ne * 8 + arr[i];
            if (!seen[ne]) { seen[ne] = 1; queue[t++] = ne; }
            tmp = arr[a]; arr[a] = arr[b]; arr[b] = tmp;
        }
    }
    int x = best; for (int i = n - 1; i >= 0; i--) { out[i] = x % 8; x /= 8; }
    free(queue);
}

static int self_check(int trials) {
    int bad = 0;
    for (int t = 0; t < trials; t++) {
        int n = 1 + rnd_int(7), m = rnd_int(8), by_value = rnd_int(2);
        int A[8], eu[8], ev[8], o1[8], o2[8];
        for (int i = 0; i < n; i++) A[i] = i;                          
        for (int i = n - 1; i > 0; i--) { int j = rnd_int(i + 1); int x = A[i]; A[i] = A[j]; A[j] = x; }
        for (int e = 0; e < m; e++) { eu[e] = rnd_int(n); ev[e] = rnd_int(n); }
        smallest_perm(n, A, m, eu, ev, by_value, o1); brute(n, A, m, eu, ev, by_value, o2);
        for (int i = 0; i < n; i++) if (o1[i] != o2[i]) { bad++; break; }
    }
    return bad;
}

static void print_arr(const char *label, const int *a, int n, int off) {
    printf("%s", label); for (int i = 0; i < n; i++) printf("%d ", a[i] + off); printf("\n");
}

static void demo(void) {
    int A[] = {5, 4, 3, 2, 1, 6}; int n = 6;                        
    int eu[] = {0, 2, 3}, ev[] = {4, 3, 5};                         
    int out[6];
    for (int i = 0; i < n; i++) A[i]--;                              
    print_arr("A = ", A, n, 1);
    printf("Allowed swaps (positions): (1,5) (3,4) (4,6)\n");
    smallest_perm(n, A, 3, eu, ev, 0, out);
    print_arr("Lexicographically smallest reachable permutation: ", out, n, 1);
    printf("  components: {1,5} holds {5,1} -> 1 at pos 1, 5 at pos 5;  {3,4,6} holds {3,2,6} -> 2,3,6;  {2} stays 4\n");
    int bad = self_check(3000);
    printf("Self-check vs exhaustive search of reachable permutations (3000 random cases, both swap modes): %s (%d mismatches)\n\n", bad ? "FAILED" : "PASSED", bad);
}

static void run_interactive(void) {
    int n, m, mode;
    printf("Enter n (permutation of 1..n): "); if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *A = malloc(n * sizeof(int)), *seen = calloc(n + 1, sizeof(int));
    printf("Enter the permutation (%d numbers):\n", n);
    for (int i = 0; i < n; i++) {
        printf("A[%d]: ", i + 1);
        if (scanf("%d", &A[i]) != 1 || A[i] < 1 || A[i] > n || seen[A[i]]) { printf("Invalid (must be a permutation of 1..n).\n"); free(A); free(seen); return; }
        seen[A[i]] = 1; A[i]--;
    }
    printf("Do swaps refer to  1 = positions  or  2 = values?  "); if (scanf("%d", &mode) != 1 || (mode != 1 && mode != 2)) { free(A); free(seen); return; }
    printf("Enter number of allowed swaps m: "); if (scanf("%d", &m) != 1 || m < 0) { free(A); free(seen); return; }
    int *eu = malloc((m + 1) * sizeof(int)), *ev = malloc((m + 1) * sizeof(int));
    printf("Enter the swaps as: u v   (numbers 1..%d)\n", n);
    for (int i = 0; i < m; i++) {
        printf("Swap %d: ", i + 1);
        if (scanf("%d %d", &eu[i], &ev[i]) != 2 || eu[i] < 1 || eu[i] > n || ev[i] < 1 || ev[i] > n) { printf("Invalid swap.\n"); free(A); free(seen); free(eu); free(ev); return; }
        eu[i]--; ev[i]--;
    }
    int *out = malloc(n * sizeof(int));
    smallest_perm(n, A, m, eu, ev, mode == 2, out);
    print_arr("\nLexicographically smallest permutation: ", out, n, 1);
    free(A); free(seen); free(eu); free(ev); free(out);
}

static void run_benchmark(void) {
    FILE *fp = fopen("Lexicographically_Smallest_Connected_Component_Permutation.csv", "w");
    fprintf(fp, "n,swaps,time_ms\n");
    int sizes[] = {1000, 10000, 100000, 500000, 1000000, 2000000, 4000000};
    for (int s = 0; s < 7; s++) {
        int n = sizes[s], m = n / 2;
        int *A = malloc(n * sizeof(int)), *eu = malloc(m * sizeof(int)), *ev = malloc(m * sizeof(int)), *out = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) A[i] = i;
        for (int i = n - 1; i > 0; i--) { int j = rnd_int(i + 1); int x = A[i]; A[i] = A[j]; A[j] = x; }
        for (int e = 0; e < m; e++) { eu[e] = rnd_int(n); ev[e] = rnd_int(n); }
        clock_t t0 = clock();
        smallest_perm(n, A, m, eu, ev, 0, out);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d swaps=%8d  time=%9.3f ms\n", n, m, ms);
        fprintf(fp, "%d,%d,%.4f\n", n, m, ms);
        free(A); free(eu); free(ev); free(out);
    }
    fclose(fp);
}

int main(void) {
    printf("Q5: Lexicographically smallest permutation via swaps (DSU + per-component sort)\n");
    printf("Choose mode:\n  1 = Enter your own permutation and swaps\n  2 = Run built-in demo (+ randomized self-check)\n  3 = Run timing benchmark (writes Lexicographically_Smallest_Connected_Component_Permutation.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}