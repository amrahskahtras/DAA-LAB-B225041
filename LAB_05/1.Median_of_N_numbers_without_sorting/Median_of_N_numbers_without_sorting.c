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

double find_median(int *A, int n) {
    int *copy = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) copy[i] = A[i];

    double median;
    if (n % 2 == 1) {
        int k = n / 2; 
        median = quickselect(copy, 0, n - 1, k);
    } else {
        int k1 = n / 2 - 1, k2 = n / 2;
        int *copy2 = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) copy2[i] = A[i];
        int m1 = quickselect(copy, 0, n - 1, k1);
        int m2 = quickselect(copy2, 0, n - 1, k2);
        median = (m1 + m2) / 2.0;
        free(copy2);
    }
    free(copy);
    return median;
}

void demo(void) {
    int A[] = {12, 3, 5, 7, 4, 19, 26};      
    int B[] = {12, 3, 5, 7, 4, 19, 26, 21};  
    printf("Demo (odd n=7)  A = {12,3,5,7,4,19,26} -> median = %.1f\n", find_median(A, 7));
    printf("Demo (even n=8) B = {12,3,5,7,4,19,26,21} -> median = %.1f\n\n", find_median(B, 8));
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
    double med = find_median(A, n);
    printf("\nMedian = %.4f\n", med);
    free(A);
}

void run_benchmark(void) {
    FILE *fp = fopen("1D_array_operations_and_their_complexities.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(1);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *A = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) A[i] = rand();
        clock_t t0 = clock();
        find_median(A, n);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(A);
    }
    fclose(fp);
}

int main(void) {
    printf("Q1: Median of N numbers without sorting (randomized quickselect, O(n) avg)\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own numbers\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (Median_of_N_numbers_without_sorting.csv)\n");
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