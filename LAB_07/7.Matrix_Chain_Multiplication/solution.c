#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

long long matrixChainOrder(int *p, int n, int **s) {
    long long **m = (long long **)malloc((n) * sizeof(long long *));
    for (int i = 0; i < n; i++) {
        m[i] = (long long *)malloc((n) * sizeof(long long));
        for (int j = 0; j < n; j++) m[i][j] = 0;
    }

    for (int len = 2; len < n; len++) {
        for (int i = 1; i < n - len + 1; i++) {
            int j = i + len - 1;
            m[i][j] = LLONG_MAX;
            for (int k = i; k < j; k++) {
                long long cost = m[i][k] + m[k + 1][j] + (long long)p[i - 1] * p[k] * p[j];
                if (cost < m[i][j]) {
                    m[i][j] = cost;
                    s[i][j] = k;
                }
            }
        }
    }

    long long result = m[1][n - 1];
    for (int i = 0; i < n; i++) free(m[i]);
    free(m);
    return result;
}

void printOptimalParens(int **s, int i, int j) {
    if (i == j) {
        printf("A%d", i);
    } else {
        printf("(");
        printOptimalParens(s, i, s[i][j]);
        printOptimalParens(s, s[i][j] + 1, j);
        printf(")");
    }
}

void generateRandomDims(int *p, int n, int seed) {
    srand(seed);
    for (int i = 0; i < n; i++) {
        p[i] = 10 + rand() % 90;
    }
}

int main(int argc, char *argv[]) {
    int demoP[] = {30, 35, 15, 5, 10, 20, 25};
    int demoN = 7;

    int **s = (int **)malloc(demoN * sizeof(int *));
    for (int i = 0; i < demoN; i++) s[i] = (int *)calloc(demoN, sizeof(int));

    long long minCost = matrixChainOrder(demoP, demoN, s);
    printf("Demo matrices (dims: 30x35 35x15 15x5 5x10 10x20 20x25)\n");
    printf("Minimum scalar multiplications: %lld\n", minCost);
    printf("Optimal parenthesization: ");
    printOptimalParens(s, 1, demoN - 1);
    printf("\n\n");

    for (int i = 0; i < demoN; i++) free(s[i]);
    free(s);

    int sizes[] = {5, 10, 20, 50, 100, 150, 200, 300, 400, 500};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    FILE *fp = fopen("data.csv", "w");
    if (!fp) {
        printf("Error opening data.csv\n");
        return 1;
    }

    fprintf(fp, "n_matrices,min_scalar_mults,time_ms\n");
    printf("n_matrices\tmin_scalar_mults\ttime_ms\n");

    for (int idx = 0; idx < numSizes; idx++) {
        int n = sizes[idx];
        int *p = (int *)malloc((n + 1) * sizeof(int));
        generateRandomDims(p, n + 1, 7);

        int **sMat = (int **)malloc((n + 1) * sizeof(int *));
        for (int i = 0; i < n + 1; i++) sMat[i] = (int *)calloc(n + 1, sizeof(int));

        clock_t start = clock();
        long long cost = matrixChainOrder(p, n + 1, sMat);
        clock_t end = clock();
        double timeMs = 1000.0 * (end - start) / CLOCKS_PER_SEC;

        fprintf(fp, "%d,%lld,%.4f\n", n, cost, timeMs);
        printf("%d\t\t%lld\t\t%.4f\n", n, cost, timeMs);

        for (int i = 0; i < n + 1; i++) free(sMat[i]);
        free(sMat);
        free(p);
    }

    fclose(fp);
    return 0;
}
