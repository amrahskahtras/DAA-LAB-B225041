#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double obst(double *p, double *q, int n, int **root_table) {
    double **e = malloc((n + 2) * sizeof(double *));
    double **w = malloc((n + 2) * sizeof(double *));
    for (int i = 0; i < n + 2; i++) { e[i] = malloc((n + 1) * sizeof(double)); w[i] = malloc((n + 1) * sizeof(double)); }

    for (int i = 1; i <= n + 1; i++) { e[i][i-1] = q[i-1]; w[i][i-1] = q[i-1]; }

    for (int len = 1; len <= n; len++) {
        for (int i = 1; i <= n - len + 1; i++) {
            int j = i + len - 1;
            e[i][j] = 1e18;
            w[i][j] = w[i][j-1] + p[j] + q[j];
            for (int r = i; r <= j; r++) {
                double cost = e[i][r-1] + e[r+1][j] + w[i][j];
                if (cost < e[i][j]) {
                    e[i][j] = cost;
                    if (root_table) root_table[i][j] = r;
                }
            }
        }
    }
    double result = e[1][n];
    for (int i = 0; i < n + 2; i++) { free(e[i]); free(w[i]); }
    free(e); free(w);
    return result;
}

void print_tree_structure(int **root, int i, int j, int parent_key, const char *side) {
    if (i > j) { if (parent_key != -1) printf("  d%d is %s child of k%d\n", i - 1, side, parent_key); return; }
    int r = root[i][j];
    if (parent_key == -1) printf("  k%d is the ROOT\n", r);
    else printf("  k%d is %s child of k%d\n", r, side, parent_key);
    print_tree_structure(root, i, r - 1, r, "left");
    print_tree_structure(root, r + 1, j, r, "right");
}

void demo(void) {
    int n = 5;
    double p[] = {0, 0.15, 0.10, 0.05, 0.10, 0.20};   
    double q[] = {0.05, 0.10, 0.05, 0.05, 0.05, 0.10}; 

    int **root = malloc((n + 2) * sizeof(int *));
    for (int i = 0; i < n + 2; i++) { root[i] = malloc((n + 1) * sizeof(int)); for (int j = 0; j <= n; j++) root[i][j] = -1; }

    double cost = obst(p, q, n, root);
    printf("n=%d keys, expected search cost of optimal BST = %.4f (expected ~2.75)\n", n, cost);
    printf("Tree structure:\n");
    print_tree_structure(root, 1, n, -1, "");
    printf("\n");

    for (int i = 0; i < n + 2; i++) free(root[i]);
    free(root);
}

void run_interactive(void) {
    int n;
    printf("Enter number of keys n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    double *p = malloc((n + 1) * sizeof(double));
    double *q = malloc((n + 1) * sizeof(double));
    printf("Enter p[1..%d] (search probabilities of the keys):\n", n);
    for (int i = 1; i <= n; i++) { printf("p[%d]: ", i); if (scanf("%lf", &p[i]) != 1) { free(p); free(q); return; } }
    printf("Enter q[0..%d] (probabilities of unsuccessful searches):\n", n);
    for (int i = 0; i <= n; i++) { printf("q[%d]: ", i); if (scanf("%lf", &q[i]) != 1) { free(p); free(q); return; } }

    int **root = malloc((n + 2) * sizeof(int *));
    for (int i = 0; i < n + 2; i++) { root[i] = malloc((n + 1) * sizeof(int)); for (int j = 0; j <= n; j++) root[i][j] = -1; }

    double cost = obst(p, q, n, root);
    printf("\nMinimum expected search cost = %.4f\nTree structure:\n", cost);
    print_tree_structure(root, 1, n, -1, "");

    for (int i = 0; i < n + 2; i++) free(root[i]);
    free(root); free(p); free(q);
}

void run_benchmark(void) {
    FILE *fp = fopen("Optimal_Binary_Search_Trees.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {10, 20, 40, 60, 90, 130, 180, 240, 320};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(8);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        double *p = malloc((n + 1) * sizeof(double));
        double *q = malloc((n + 1) * sizeof(double));
        double total = 0;
        for (int i = 1; i <= n; i++) { p[i] = 1 + rand() % 100; total += p[i]; }
        for (int i = 0; i <= n; i++) { q[i] = 1 + rand() % 20; total += q[i]; }
        for (int i = 1; i <= n; i++) p[i] /= total;
        for (int i = 0; i <= n; i++) q[i] /= total;

        clock_t t0 = clock();
        obst(p, q, n, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%4d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(p); free(q);
    }
    fclose(fp);
}

int main(void) {
    printf("Q8: Optimal Binary Search Tree (DP, O(n^3))\n");
    printf("Choose mode:\n  1 = Enter your own keys/probabilities\n  2 = Run built-in demo (CLRS example)\n  3 = Run timing benchmark (writes Optimal_Binary_Search_Trees.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
