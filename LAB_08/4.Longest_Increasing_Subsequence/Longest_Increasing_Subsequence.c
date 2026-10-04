#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int lower_bound(int *tails, int len, int key) {
    int lo = 0, hi = len;
    while (lo < hi) { int mid = (lo + hi) / 2; if (tails[mid] < key) lo = mid + 1; else hi = mid; }
    return lo;
}


int lis_nlogn(int *A, int n, int *out_lis) {
    int *tails = malloc(n * sizeof(int));       
    int *parent = malloc(n * sizeof(int));
    int len = 0;
    for (int i = 0; i < n; i++) {
        int lo = 0, hi = len;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (A[tails[mid]] < A[i]) lo = mid + 1; else hi = mid;
        }
        parent[i] = (lo > 0) ? tails[lo - 1] : -1;
        tails[lo] = i;
        if (lo == len) len++;
    }
    if (out_lis) {
        int idx = tails[len - 1], pos = len;
        out_lis[len] = -1; 
        while (idx != -1) { out_lis[--pos] = A[idx]; idx = parent[idx]; }
    }
    free(tails); free(parent);
    return len;
}

void demo(void) {
    int A[] = {10, 9, 2, 5, 3, 7, 101, 18};
    int n = 8;
    int lis[64];
    int len = lis_nlogn(A, n, lis);
    printf("A = [10,9,2,5,3,7,101,18] -> LIS length = %d, LIS = [", len);
    for (int i = 0; i < len; i++) printf("%d%s", lis[i], i+1<len?",":"");
    printf("] (expected length 4, e.g. [2,3,7,18] or [2,5,7,101])\n\n");
}

void run_interactive(void) {
    int n;
    printf("Enter n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *A = malloc(n * sizeof(int));
    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) { printf("A[%d]: ", i); if (scanf("%d", &A[i]) != 1) { free(A); return; } }
    int *lis = malloc(n * sizeof(int));
    int len = lis_nlogn(A, n, lis);
    printf("\nLIS length = %d\nLIS = [", len);
    for (int i = 0; i < len; i++) printf("%d%s", lis[i], i+1<len?",":"");
    printf("]\n");
    free(A); free(lis);
}

void run_benchmark(void) {
    FILE *fp = fopen("Longest_Increasing_Subsequence.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(4);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *A = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) A[i] = rand();
        clock_t t0 = clock();
        lis_nlogn(A, n, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(A);
    }
    fclose(fp);
}

int main(void) {
    printf("Q4: Longest Increasing Subsequence (patience sorting, O(n log n))\n");
    printf("Choose mode:\n  1 = Enter your own array\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Longest_Increasing_Subsequence.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
