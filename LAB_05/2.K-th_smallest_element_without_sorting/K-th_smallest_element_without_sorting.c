#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap_int(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int partition_random(int *A, int lo, int hi) {
    int pivot_idx = lo + rand() % (hi - lo + 1);
    swap_int(&A[pivot_idx], &A[hi]);
    int pivot = A[hi];
    int store = lo;
    for (int i = lo; i < hi; i++) {
        if (A[i] < pivot) { swap_int(&A[i], &A[store]); store++; }
    }
    swap_int(&A[store], &A[hi]);
    return store;
}

int quickselect(int *A, int lo, int hi, int k) {
    while (lo < hi) {
        int p = partition_random(A, lo, hi);
        if (p == k) return A[p];
        else if (k < p) hi = p - 1;
        else lo = p + 1;
    }
    return A[lo];
}

int kth_smallest(int *A, int n, int K) {
    int *copy = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) copy[i] = A[i];
    int result = quickselect(copy, 0, n - 1, K - 1);
    free(copy);
    return result;
}

void demo(void) {
    int A[] = {12, 3, 5, 7, 4, 19, 26};
    int K = 3;
    printf("Demo: A = {12,3,5,7,4,19,26}, K=%d -> %d-th smallest = %d\n\n",
           K, K, kth_smallest(A, 7, K));
}

void run_interactive(void) {
    int n;
    printf("Enter N (number of elements): ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid N.\n"); return; }
    int *A = malloc(n * sizeof(int));
    printf("Enter %d numbers:\n", n);
    for (int i = 0; i < n; i++) {
        printf("A[%d]: ", i + 1);
        if (scanf("%d", &A[i]) != 1) { printf("Invalid input.\n"); free(A); return; }
    }
    int K;
    printf("Enter K (1 <= K <= %d): ", n);
    if (scanf("%d", &K) != 1 || K < 1 || K > n) { printf("Invalid K.\n"); free(A); return; }
    printf("\n%d-th smallest element = %d\n", K, kth_smallest(A, n, K));
    free(A);
}

void run_benchmark(void) {
    FILE *fp = fopen("K-th_smallest_element_without_sorting.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(2);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *A = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) A[i] = rand();
        int K = n / 3 + 1; /* arbitrary fixed rank to query */
        clock_t t0 = clock();
        kth_smallest(A, n, K);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(A);
    }
    fclose(fp);
}

int main(void) {
    printf("Q2: K-th smallest element without sorting (randomized quickselect, O(n) avg)\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own numbers and K\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes K-th_smallest_element_without_sorting.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    srand((unsigned)time(NULL));
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
