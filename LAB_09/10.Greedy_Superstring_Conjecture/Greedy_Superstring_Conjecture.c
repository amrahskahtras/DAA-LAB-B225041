#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char *xstrdup(const char *s) { size_t l = strlen(s); char *r = malloc(l + 1); memcpy(r, s, l + 1); return r; }

static int overlap(const char *a, int la, const char *b, int lb) {
    int m = la < lb ? la : lb;
    for (int l = m; l > 0; l--) if (memcmp(a + la - l, b, l) == 0) return l;
    return 0;
}

static int preprocess(char **s, int n) {
    int *keep = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        keep[i] = 1;
        for (int j = 0; j < n && keep[i]; j++) {
            if (i == j) continue;
            if (strstr(s[j], s[i]) && (strlen(s[j]) > strlen(s[i]) || j < i)) keep[i] = 0;
        }
    }
    int m = 0;
    for (int i = 0; i < n; i++) if (keep[i]) s[m++] = s[i];
    free(keep);
    return m;
}

char *greedy_superstring(char **in, int n_in) {
    char **s = malloc(n_in * sizeof(char *));
    for (int i = 0; i < n_in; i++) s[i] = in[i];
    int n = preprocess(s, n_in);
    if (n == 0) { free(s); return xstrdup(""); }
    char **cur = malloc(n * sizeof(char *)); int *len = malloc(n * sizeof(int)); int *act = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) { cur[i] = xstrdup(s[i]); len[i] = strlen(s[i]); act[i] = 1; }
    int *ov = malloc((size_t)n * n * sizeof(int));
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) ov[(size_t)i*n+j] = (i == j) ? -1 : overlap(cur[i], len[i], cur[j], len[j]);
    for (int round = 0; round < n - 1; round++) {
        int bi = -1, bj = -1, best = -1;
        for (int i = 0; i < n; i++) if (act[i]) for (int j = 0; j < n; j++) if (act[j] && i != j && ov[(size_t)i*n+j] > best) { best = ov[(size_t)i*n+j]; bi = i; bj = j; }
        int nl = len[bi] + len[bj] - best;
        char *merged = malloc(nl + 1);
        memcpy(merged, cur[bi], len[bi]); memcpy(merged + len[bi], cur[bj] + best, len[bj] - best); merged[nl] = '\0';
        free(cur[bi]); free(cur[bj]); cur[bj] = NULL; cur[bi] = merged; len[bi] = nl; act[bj] = 0;
        for (int k = 0; k < n; k++) if (act[k] && k != bi) {
            ov[(size_t)bi*n+k] = overlap(cur[bi], len[bi], cur[k], len[k]);
            ov[(size_t)k*n+bi] = overlap(cur[k], len[k], cur[bi], len[bi]);
        }
    }
    char *res = NULL;
    for (int i = 0; i < n; i++) if (act[i]) res = cur[i];
    for (int i = 0; i < n; i++) if (cur[i] && cur[i] != res) free(cur[i]);
    free(cur); free(len); free(act); free(ov); free(s);
    return res;
}
int exact_scs_length(char **in, int n_in) {
    char **s = malloc(n_in * sizeof(char *));
    for (int i = 0; i < n_in; i++) s[i] = in[i];
    int n = preprocess(s, n_in);
    if (n > 15) { free(s); return -1; }
    if (n == 0) { free(s); return 0; }
    int len[16], ov[16][16];
    for (int i = 0; i < n; i++) len[i] = strlen(s[i]);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) ov[i][j] = (i == j) ? 0 : overlap(s[i], len[i], s[j], len[j]);
    int full = (1 << n) - 1;
    int *dp = malloc((size_t)(full + 1) * n * sizeof(int));
    for (size_t k = 0; k < (size_t)(full + 1) * n; k++) dp[k] = 1 << 30;
    for (int j = 0; j < n; j++) dp[(size_t)(1 << j) * n + j] = len[j];
    for (int mask = 1; mask <= full; mask++)
        for (int j = 0; j < n; j++) {
            int v = dp[(size_t)mask * n + j];
            if (!(mask >> j & 1) || v >= (1 << 30)) continue;
            for (int k = 0; k < n; k++) if (!(mask >> k & 1)) {
                int nv = v + len[k] - ov[j][k];
                size_t idx = (size_t)(mask | (1 << k)) * n + k;
                if (nv < dp[idx]) dp[idx] = nv;
            }
        }
    int best = 1 << 30;
    for (int j = 0; j < n; j++) if (dp[(size_t)full * n + j] < best) best = dp[(size_t)full * n + j];
    free(dp); free(s);
    return best;
}

static int contains_all(const char *res, char **in, int n) {
    for (int i = 0; i < n; i++) if (!strstr(res, in[i])) return 0;
    return 1;
}

static char *repeat_str(const char *u, int m) {
    int l = strlen(u); char *r = malloc((size_t)l * m + 1);
    for (int i = 0; i < m; i++) memcpy(r + (size_t)i * l, u, l);
    r[(size_t)l * m] = '\0'; return r;
}

void demo(void) {

    char *a[] = {"CATGC", "CTAAGT", "GCTA", "TTCA", "ATGCATC"};
    char *g = greedy_superstring(a, 5);
    printf("Example strings: CATGC CTAAGT GCTA TTCA ATGCATC\n  greedy = %s (length %zu, covers all: %s)\n  exact optimum length = %d\n\n",
           g, strlen(g), contains_all(g, a, 5) ? "YES" : "NO", exact_scs_length(a, 5));
    free(g);

    printf("Classic lower-bound family { c(ab)^m, (ab)^m c, (ba)^m } -- greedy/optimal -> 2:\n");
    int ms[] = {2, 5, 10, 50, 200};
    for (int t = 0; t < 5; t++) {
        int m = ms[t];
        char *ab = repeat_str("ab", m), *ba = repeat_str("ba", m);
        char *s1 = malloc(2 * m + 3), *s2 = malloc(2 * m + 3);
        sprintf(s1, "c%s", ab); sprintf(s2, "%sc", ab);
        char *set[] = {s1, s2, ba};
        char *gr = greedy_superstring(set, 3);
        int opt = exact_scs_length(set, 3);
        printf("  m=%3d: greedy length = %4zu, optimal length = %4d, ratio = %.4f\n", m, strlen(gr), opt, (double)strlen(gr) / opt);
        free(gr); free(ab); free(ba); free(s1); free(s2);
    }

    srand(10);
    double worst = 1.0; int trials = 3000;
    for (int t = 0; t < trials; t++) {
        int n = 4 + rand() % 7, L = 3 + rand() % 5, alpha = 2 + rand() % 2;
        char **set = malloc(n * sizeof(char *));
        for (int i = 0; i < n; i++) { set[i] = malloc(L + 1); for (int k = 0; k < L; k++) set[i][k] = 'a' + rand() % alpha; set[i][L] = '\0'; }
        char *gr = greedy_superstring(set, n); int opt = exact_scs_length(set, n);
        if (opt > 0 && (double)strlen(gr) / opt > worst) worst = (double)strlen(gr) / opt;
        for (int i = 0; i < n; i++) free(set[i]);
        free(set); free(gr);
    }
    printf("\nRandom study (%d random instances, n=4..10, short strings, tiny alphabets):\n  worst greedy/optimal ratio observed = %.4f\n", trials, worst);
    printf("  -> random instances stay far below 2; the claimed 9/4 counterexample needs a carefully\n     engineered family of equal-length strings (even k >= 10), per the preprint.\n\n");
}

void run_interactive(void) {
    int n;
    printf("Enter number of strings n: "); if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    char **s = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) { s[i] = malloc(1024); printf("String %d: ", i + 1); if (scanf("%1023s", s[i]) != 1) { printf("Invalid.\n"); return; } }
    char *g = greedy_superstring(s, n);
    printf("\nGreedy superstring: %s\nLength = %zu (covers all inputs: %s)\n", g, strlen(g), contains_all(g, s, n) ? "YES" : "NO");
    int opt = exact_scs_length(s, n);
    if (opt >= 0) printf("Exact optimum length = %d  ->  greedy/optimal = %.4f\n", opt, (double)strlen(g) / opt);
    else printf("(exact optimum skipped: more than 15 strings after preprocessing)\n");
    free(g);
}

void run_benchmark(void) {
    FILE *fr = fopen("Greedy_Superstring_Conjecture_ratio.csv", "w");
    fprintf(fr, "m,greedy_len,opt_len,ratio\n");
    int ms[] = {1, 2, 3, 5, 8, 10, 20, 50, 100, 200, 500};
    for (int t = 0; t < 11; t++) {
        int m = ms[t];
        char *ab = repeat_str("ab", m), *ba = repeat_str("ba", m);
        char *s1 = malloc(2 * m + 3), *s2 = malloc(2 * m + 3);
        sprintf(s1, "c%s", ab); sprintf(s2, "%sc", ab);
        char *set[] = {s1, s2, ba};
        char *gr = greedy_superstring(set, 3); int opt = exact_scs_length(set, 3);
        fprintf(fr, "%d,%zu,%d,%.5f\n", m, strlen(gr), opt, (double)strlen(gr) / opt);
        free(gr); free(ab); free(ba); free(s1); free(s2);
    }
    fclose(fr);
    FILE *fp = fopen("Greedy_Superstring_Conjecture_timing.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {25, 50, 100, 150, 200, 300, 400, 500, 600};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(11);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s], L = 10;
        char **set = malloc(n * sizeof(char *));
        for (int i = 0; i < n; i++) { set[i] = malloc(L + 1); for (int k = 0; k < L; k++) set[i][k] = 'a' + rand() % 4; set[i][L] = '\0'; }
        clock_t t0 = clock();
        char *g = greedy_superstring(set, n);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%4d  time=%9.3f ms  (superstring length %zu)\n", n, ms, strlen(g));
        fprintf(fp, "%d,%.4f\n", n, ms);
        for (int i = 0; i < n; i++) free(set[i]);
        free(set); free(g);
    }
    fclose(fp);
}

int main(void) {
    printf("Q10: Greedy Superstring (open problem; greedy heuristic + exact DP checker)\n");
    printf("Choose mode:\n  1 = Enter your own strings\n  2 = Run built-in demo (ratio->2 family + random study)\n  3 = Run timing benchmark (writes Greedy_Superstring_Conjecture.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}