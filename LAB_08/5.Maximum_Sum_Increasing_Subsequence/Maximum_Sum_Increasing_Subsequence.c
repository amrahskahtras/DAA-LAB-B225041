#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long msis(int *A, int n, int *out_seq, int *out_len) {
    long long *sum = malloc(n * sizeof(long long));
    int *parent = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) { sum[i] = A[i]; parent[i] = -1; }
    for (int i = 1; i < n; i++)
        for (int j = 0; j < i; j++)
            if (A[j] < A[i] && sum[j] + A[i] > sum[i]) { sum[i] = sum[j] + A[i]; parent[i] = j; }

    int best = 0;
    for (int i = 1; i < n; i++) if (sum[i] > sum[best]) best = i;
    long long result = sum[best];

    if (out_seq) {
        int idx = best, cnt = 0, tmp[100000];
        while (idx != -1) { tmp[cnt++] = A[idx]; idx = parent[idx]; }
        for (int i = 0; i < cnt; i++) out_seq[i] = tmp[cnt - 1 - i];
        if (out_len) *out_len = cnt;
    }
    free(sum); free(parent);
    return result;
}

void demo(void) {
    int A[] = {1, 101, 2, 3, 100, 4, 5};
    int n = 7;
    int seq[64], len;
    long long best = msis(A, n, seq, &len);
    printf("A = [1,101,2,3,100,4,5] -> Max sum increasing subsequence = %lld, sequence = [", best);
    for (int i = 0; i < len; i++) printf("%d%s", seq[i], i+1<len?",":"");
    printf("] (expected sum 106 = 1+2+3+100)\n\n");
}

void run_interactive(void) {
    int n;
    printf("Enter n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *A = malloc(n * sizeof(int));
    printf("Enter %d POSITIVE integers:\n", n);
    for (int i = 0; i < n; i++) { printf("A[%d]: ", i); if (scanf("%d", &A[i]) != 1) { free(A); return; } }
    int *seq = malloc(n * sizeof(int)); int len;
    long long best = msis(A, n, seq, &len);
    printf("\nMax sum increasing subsequence = %lld\nSequence = [", best);
    for (int i = 0; i < len; i++) printf("%d%s", seq[i], i+1<len?",":"");
    printf("]\n");
    free(A); free(seq);
}

void run_benchmark(void) {
    FILE *fp = fopen("Maximum_Sum_Increasing_Subsequence.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {200, 500, 1000, 2000, 4000, 6000, 8000, 10000, 14000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(5);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *A = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) A[i] = 1 + rand() % 100000;
        clock_t t0 = clock();
        msis(A, n, NULL, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%6d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(A);
    }
    fclose(fp);
}

int main(void) {
    printf("Q5: Maximum Sum Increasing Subsequence (DP, O(n^2))\n");
    printf("Choose mode:\n  1 = Enter your own array\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Maximum_Sum_Increasing_Subsequence.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
