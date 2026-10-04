#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int lcs_length_and_string(const char *X, const char *Y, char *out_lcs) {
    int m = strlen(X), n = strlen(Y);
    int **dp = malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) dp[i] = calloc(n + 1, sizeof(int));

    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (X[i-1] == Y[j-1]) dp[i][j] = dp[i-1][j-1] + 1;
            else dp[i][j] = (dp[i-1][j] > dp[i][j-1]) ? dp[i-1][j] : dp[i][j-1];
        }

    int len = dp[m][n];
    if (out_lcs) {
        int i = m, j = n, pos = len;
        out_lcs[len] = '\0';
        while (i > 0 && j > 0) {
            if (X[i-1] == Y[j-1]) { out_lcs[--pos] = X[i-1]; i--; j--; }
            else if (dp[i-1][j] >= dp[i][j-1]) i--;
            else j--;
        }
    }
    for (int i = 0; i <= m; i++) free(dp[i]);
    free(dp);
    return len;
}

void demo(void) {
    const char *X = "AGGTAB";
    const char *Y = "GXTXAYB";
    char lcs[64];
    int len = lcs_length_and_string(X, Y, lcs);
    printf("X = \"%s\", Y = \"%s\"\n", X, Y);
    printf("LCS length = %d, LCS = \"%s\" (expected length 4, e.g. \"GTAB\")\n\n", len, lcs);
}

void run_interactive(void) {
    char X[1024], Y[1024];
    printf("Enter string X: ");
    if (scanf("%1023s", X) != 1) { printf("Invalid.\n"); return; }
    printf("Enter string Y: ");
    if (scanf("%1023s", Y) != 1) { printf("Invalid.\n"); return; }
    char *lcs = malloc(strlen(X) + strlen(Y) + 1);
    int len = lcs_length_and_string(X, Y, lcs);
    printf("\nLCS length = %d\nLCS = \"%s\"\n", len, lcs);
    free(lcs);
}

void run_benchmark(void) {
    FILE *fp = fopen("Longest_Common_Subsequence.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {100, 200, 400, 800, 1200, 1600, 2400, 3200};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(3);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        char *X = malloc(n + 1), *Y = malloc(n + 1);
        for (int i = 0; i < n; i++) { X[i] = 'A' + rand() % 4; Y[i] = 'A' + rand() % 4; }
        X[n] = Y[n] = '\0';
        char *lcs = malloc(2 * n + 2);
        clock_t t0 = clock();
        lcs_length_and_string(X, Y, lcs);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%6d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(X); free(Y); free(lcs);
    }
    fclose(fp);
}

int main(void) {
    printf("Q3: Longest Common Subsequence (DP, O(m*n))\n");
    printf("Choose mode:\n  1 = Enter your own X, Y\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Longest_Common_Subsequence.csv, m=n)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
