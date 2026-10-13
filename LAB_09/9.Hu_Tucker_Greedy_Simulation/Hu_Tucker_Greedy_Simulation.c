#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef long long ll;

ll hu_tucker(ll *w, int n, int *level) {
    if (n == 1) { level[0] = 0; return 0; }
    int total = 2 * n;
    int *lc = malloc(total * sizeof(int)), *rc = malloc(total * sizeof(int));
    ll *wt = malloc(total * sizeof(ll));
    int *seq = malloc(n * sizeof(int)); 
    for (int i = 0; i < n; i++) { wt[i] = w[i]; lc[i] = rc[i] = -1; seq[i] = i; }
    int m = n, next_id = n;
    while (m > 1) {
        int best_i = -1, best_j = -1; ll best_sum = 0;
        int win_j = -1;            
        for (int i = m - 2; i >= 0; i--) {
            int j_new = i + 1;
            if (seq[j_new] < n) win_j = j_new;                    
            else if (win_j == -1 || wt[seq[j_new]] <= wt[seq[win_j]]) win_j = j_new; 
            ll s = wt[seq[i]] + wt[seq[win_j]];
            if (best_i == -1 || s <= best_sum) { best_sum = s; best_i = i; best_j = win_j; }
        }
        int a = seq[best_i], b = seq[best_j];
        int id = next_id++;
        wt[id] = wt[a] + wt[b]; lc[id] = a; rc[id] = b;
        seq[best_j] = id;                                           
        for (int k = best_i; k < m - 1; k++) seq[k] = seq[k + 1];   
        m--;
    }
    int root = seq[0];
    int *depth = calloc(next_id, sizeof(int));
    for (int v = root; v >= 0; v--) if (lc[v] != -1) { depth[lc[v]] = depth[v] + 1; depth[rc[v]] = depth[v] + 1; }
    ll cost = 0;
    for (int i = 0; i < n; i++) { level[i] = depth[i]; cost += w[i] * depth[i]; }
    free(lc); free(rc); free(wt); free(seq); free(depth);
    return cost;
}

int recombine(int *level, int n, char *out, size_t cap) {
    if (n <= 0) { out[0] = '\0'; return 0; }
    char **sub = malloc(n * sizeof(char *)); int *lv = malloc(n * sizeof(int)); int top = 0;
    for (int i = 0; i < n; i++) {
        char *leaf = malloc(16); snprintf(leaf, 16, "%d", i + 1);
        sub[top] = leaf; lv[top] = level[i]; top++;
        while (top >= 2 && lv[top-1] == lv[top-2]) {
            size_t L = strlen(sub[top-2]) + strlen(sub[top-1]) + 4;
            char *merged = malloc(L); snprintf(merged, L, "(%s %s)", sub[top-2], sub[top-1]);
            free(sub[top-2]); free(sub[top-1]);
            top--; sub[top-1] = merged; lv[top-1] = lv[top] - 1;
        }
    }
    int ok = (top == 1 && lv[0] == 0);
    snprintf(out, cap, "%s", sub[0]);
    for (int i = 0; i < top; i++) free(sub[i]);
    free(sub); free(lv);
    return ok;
}
ll alphabetic_dp(ll *w, int n) {
    ll *pre = malloc((n + 1) * sizeof(ll)); pre[0] = 0;
    for (int i = 0; i < n; i++) pre[i+1] = pre[i] + w[i];
    ll *C = calloc((size_t)n * n, sizeof(ll));
    for (int len = 2; len <= n; len++)
        for (int i = 0; i + len - 1 < n; i++) {
            int j = i + len - 1; ll best = -1;
            for (int k = i; k < j; k++) {
                ll c = C[i*n+k] + C[(k+1)*n+j];
                if (best < 0 || c < best) best = c;
            }
            C[i*n+j] = best + pre[j+1] - pre[i];
        }
    ll r = C[0*n + n-1]; free(C); free(pre);
    return r;
}

void demo(void) {
    ll w[] = {1, 2, 3, 4, 5, 6, 7, 8};   
    int n = 8, level[8]; char tree[512];
    ll c = hu_tucker(w, n, level);
    int ok = recombine(level, n, tree, sizeof tree);
    printf("w = {1,2,3,4,5,6,7,8}\nlevels = "); for (int i = 0; i < n; i++) printf("%d ", level[i]);
    printf("\nHu-Tucker cost = %lld, DP optimum = %lld (%s)\nOrdered tree: %s  [phase-3 consistent=%s]\n\n",
           c, alphabetic_dp(w, n), c == alphabetic_dp(w, n) ? "MATCH" : "MISMATCH", tree, ok ? "YES" : "NO");

    ll w2[] = {9, 1, 1, 9, 1, 1, 9};      
    c = hu_tucker(w2, n, level); ok = recombine(level, n, tree, sizeof tree);
    printf("w = {9,1,1,9,1,1,9}\nlevels = "); for (int i = 0; i < n; i++) printf("%d ", level[i]);
    printf("\nHu-Tucker cost = %lld, DP optimum = %lld (%s)\nOrdered tree: %s  [phase-3 consistent=%s]\n\n",
           c, alphabetic_dp(w2, n), c == alphabetic_dp(w2, n) ? "MATCH" : "MISMATCH", tree, ok ? "YES" : "NO");
}

void run_interactive(void) {
    int n;
    printf("Enter number of weights n: "); if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    ll *w = malloc(n * sizeof(ll)); int *level = malloc(n * sizeof(int));
    printf("Enter %d positive weights IN ORDER:\n", n);
    for (int i = 0; i < n; i++) { printf("w[%d]: ", i + 1); if (scanf("%lld", &w[i]) != 1 || w[i] <= 0) { printf("Invalid.\n"); free(w); free(level); return; } }
    ll c = hu_tucker(w, n, level);
    char *tree = malloc(64 * (size_t)n + 64);
    int ok = recombine(level, n, tree, 64 * (size_t)n + 64);
    printf("\nLeaf levels (depths): "); for (int i = 0; i < n; i++) printf("%d ", level[i]);
    printf("\nMinimum weighted path length = %lld\nOrdered tree (leaf numbers = input positions): %s\n", c, tree);
    if (n <= 400) printf("Cross-check with exact O(n^3) DP = %lld (%s)\n", alphabetic_dp(w, n), alphabetic_dp(w, n) == c ? "MATCH" : "MISMATCH");
    (void)ok; free(w); free(level); free(tree);
}

void run_benchmark(void) {
    FILE *fp = fopen("Hu_Tucker_Greedy_Simulation.csv", "w");
    fprintf(fp, "n,hu_tucker_ms,dp_ms\n");
    int sizes[] = {100, 200, 400, 800, 1200, 1600, 2400, 3200, 4000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(9);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        ll *w = malloc(n * sizeof(ll)); int *level = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) w[i] = 1 + rand() % 1000;
        clock_t t0 = clock(); ll c = hu_tucker(w, n, level); clock_t t1 = clock();
        double ht = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC, dp = -1;
        if (n <= 800) { t0 = clock(); ll d = alphabetic_dp(w, n); t1 = clock(); dp = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC; if (d != c) printf("MISMATCH at n=%d!\n", n); }
        printf("n=%5d  hu-tucker=%9.3f ms  dp=%s\n", n, ht, dp < 0 ? "skipped" : "ok");
        if (dp >= 0) fprintf(fp, "%d,%.4f,%.4f\n", n, ht, dp); else fprintf(fp, "%d,%.4f,\n", n, ht);
        free(w); free(level);
    }
    fclose(fp);
}

#ifndef NO_MAIN
int main(void) {
    printf("Q9: Hu-Tucker optimal alphabetic tree (greedy, O(n^2) implementation)\n");
    printf("Choose mode:\n  1 = Enter your own ordered weights\n  2 = Run built-in demo (cross-checked with exact DP)\n  3 = Run timing benchmark (writes Hu_Tucker_Greedy_Simulation.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
#endif