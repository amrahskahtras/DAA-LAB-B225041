#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int min3(int a, int b, int c) { int m = a; if (b < m) m = b; if (c < m) m = c; return m; }

int edit_distance(const char *A, const char *B, int print_traceback) {
    int m = strlen(A), n = strlen(B);
    int **dp = malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) dp[i] = malloc((n + 1) * sizeof(int));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;

    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (A[i-1] == B[j-1]) dp[i][j] = dp[i-1][j-1];
            else dp[i][j] = 1 + min3(dp[i-1][j-1], dp[i-1][j], dp[i][j-1]);
        }

    int result = dp[m][n];

    if (print_traceback) {
        int i = m, j = n;
        char ops[4096][64]; int cnt = 0;
        while (i > 0 || j > 0) {
            if (i > 0 && j > 0 && A[i-1] == B[j-1]) {
                snprintf(ops[cnt++], 64, "MATCH      '%c'", A[i-1]); i--; j--;
            } else if (i > 0 && j > 0 && dp[i][j] == dp[i-1][j-1] + 1) {
                snprintf(ops[cnt++], 64, "SUBSTITUTE '%c' -> '%c'", A[i-1], B[j-1]); i--; j--;
            } else if (i > 0 && dp[i][j] == dp[i-1][j] + 1) {
                snprintf(ops[cnt++], 64, "DELETE     '%c'", A[i-1]); i--;
            } else {
                snprintf(ops[cnt++], 64, "INSERT     '%c'", B[j-1]); j--;
            }
        }
        printf("Traceback (from start of strings to end):\n");
        for (int k = cnt - 1; k >= 0; k--) printf("  %s\n", ops[k]);
    }

    for (int i = 0; i <= m; i++) free(dp[i]);
    free(dp);
    return result;
}

void demo(void) {
    const char *A = "SUNDAY";
    const char *B = "SATURDAY";
    printf("A = \"%s\", B = \"%s\"\n", A, B);
    int d = edit_distance(A, B, 1);
    printf("Edit distance = %d (expected 3)\n\n", d);
}

void run_interactive(void) {
    char A[1024], B[1024];
    printf("Enter string A: ");
    if (scanf("%1023s", A) != 1) { printf("Invalid.\n"); return; }
    printf("Enter string B: ");
    if (scanf("%1023s", B) != 1) { printf("Invalid.\n"); return; }
    int d = edit_distance(A, B, 1);
    printf("\nEdit distance = %d\n", d);
}

void run_benchmark(void) {
    FILE *fp = fopen("Edit_Distance_with_Traceback_Information.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {100, 200, 400, 800, 1200, 1600, 2400, 3200};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(6);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        char *A = malloc(n + 1), *B = malloc(n + 1);
        for (int i = 0; i < n; i++) { A[i] = 'A' + rand() % 4; B[i] = 'A' + rand() % 4; }
        A[n] = B[n] = '\0';
        clock_t t0 = clock();
        edit_distance(A, B, 0);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%6d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(A); free(B);
    }
    fclose(fp);
}

int main(void) {
    printf("Q6: Edit Distance with Traceback (DP, O(m*n))\n");
    printf("Choose mode:\n  1 = Enter your own A, B\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Edit_Distance_with_Traceback_Information.csv, m=n)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
