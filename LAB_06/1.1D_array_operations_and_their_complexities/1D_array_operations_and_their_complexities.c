#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void swap_int(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int find_max(int *A, int n) {
    int mx = A[0];
    for (int i = 1; i < n; i++) if (A[i] > mx) mx = A[i];
    return mx;
}

void first_second_largest(int *A, int n, int *first, int *second) {
    int f, s;
    if (A[0] > A[1]) { f = A[0]; s = A[1]; } else { f = A[1]; s = A[0]; }
    for (int i = 2; i < n; i++) {
        if (A[i] > f) { s = f; f = A[i]; }
        else if (A[i] > s) { s = A[i]; }
    }
    *first = f; *second = s;
}

double find_mean(int *A, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) sum += A[i];
    return (double)sum / n;
}

void insertion_sort_range(int *A, int lo, int hi) {
    for (int i = lo + 1; i <= hi; i++) {
        int key = A[i], j = i - 1;
        while (j >= lo && A[j] > key) { A[j+1] = A[j]; j--; }
        A[j+1] = key;
    }
}

int partition_around_value(int *A, int lo, int hi, int value) {
    int idx = lo;
    for (int i = lo; i <= hi; i++) if (A[i] == value) { idx = i; break; }
    swap_int(&A[idx], &A[hi]);
    int pivot = A[hi], store = lo;
    for (int i = lo; i < hi; i++) if (A[i] < pivot) { swap_int(&A[i], &A[store]); store++; }
    swap_int(&A[store], &A[hi]);
    return store;
}

int mom_select(int *A, int lo, int hi, int k) {
    while (1) {
        int n = hi - lo + 1;
        if (n <= 5) {
            insertion_sort_range(A, lo, hi);
            return A[lo + k];
        }
        int numGroups = (n + 4) / 5;
        int *medians = malloc(numGroups * sizeof(int));
        for (int i = 0; i < numGroups; i++) {
            int gl = lo + i * 5, gr = gl + 4; if (gr > hi) gr = hi;
            insertion_sort_range(A, gl, gr);
            medians[i] = A[gl + (gr - gl) / 2];
        }
        int pivot = mom_select(medians, 0, numGroups - 1, numGroups / 2);
        free(medians);
        int p = partition_around_value(A, lo, hi, pivot);
        int rank = p - lo;
        if (k == rank) return A[p];
        else if (k < rank) hi = p - 1;
        else { k = k - rank - 1; lo = p + 1; }
    }
}

double find_median(int *A, int n) {
    int *copy = malloc(n * sizeof(int));
    memcpy(copy, A, n * sizeof(int));
    double med;
    if (n % 2 == 1) {
        med = mom_select(copy, 0, n - 1, n / 2);
    } else {
        int *copy2 = malloc(n * sizeof(int));
        memcpy(copy2, A, n * sizeof(int));
        int m1 = mom_select(copy, 0, n - 1, n / 2 - 1);
        int m2 = mom_select(copy2, 0, n - 1, n / 2);
        med = (m1 + m2) / 2.0;
        free(copy2);
    }
    free(copy);
    return med;
}

double find_stddev(int *A, int n) {
    double mean = find_mean(A, n);
    double sumsq = 0;
    for (int i = 0; i < n; i++) { double d = A[i] - mean; sumsq += d * d; }
    return sqrt(sumsq / n);
}

int cmp_int(const void *a, const void *b) { return (*(int*)a - *(int*)b); }

int find_mode(int *A, int n) {
    int *copy = malloc(n * sizeof(int));
    memcpy(copy, A, n * sizeof(int));
    qsort(copy, n, sizeof(int), cmp_int);
    int best_val = copy[0], best_count = 1, cur_count = 1;
    for (int i = 1; i < n; i++) {
        if (copy[i] == copy[i-1]) cur_count++; else cur_count = 1;
        if (cur_count > best_count) { best_count = cur_count; best_val = copy[i]; }
    }
    free(copy);
    return best_val;
}

int remove_duplicates(int *A, int n, int *out) {
    int *copy = malloc(n * sizeof(int));
    memcpy(copy, A, n * sizeof(int));
    qsort(copy, n, sizeof(int), cmp_int);
    int m = 0;
    for (int i = 0; i < n; i++) if (i == 0 || copy[i] != copy[i-1]) out[m++] = copy[i];
    free(copy);
    return m;
}

void reverse_array(int *A, int n) {
    int i = 0, j = n - 1;
    while (i < j) { swap_int(&A[i], &A[j]); i++; j--; }
}

int partition_ge_first(int *A, int n, int pivot) {
    int store = 0;
    for (int i = 0; i < n; i++) {
        if (A[i] >= pivot) { swap_int(&A[i], &A[store]); store++; }
    }
    return store;
}

void print_array(int *A, int n) { for (int i = 0; i < n; i++) printf("%d ", A[i]); printf("\n"); }

void demo(void) {
    int A[] = {12, 45, 3, 45, 7, 3, 89, 1, 45, 23};
    int n = 10;
    printf("Array: "); print_array(A, n);

    printf("(i)   Max = %d\n", find_max(A, n));

    int f, s; first_second_largest(A, n, &f, &s);
    printf("(ii)  First largest = %d, Second largest = %d\n", f, s);

    printf("(iii) Mean = %.3f\n", find_mean(A, n));
    printf("(iv)  Median = %.3f\n", find_median(A, n));
    printf("(v)   Std Dev = %.3f\n", find_stddev(A, n));
    printf("(vi)  Mode = %d\n", find_mode(A, n));

    int *uniq = malloc(n * sizeof(int));
    int m = remove_duplicates(A, n, uniq);
    printf("(vii) Unique (sorted) = "); print_array(uniq, m);
    free(uniq);

    int B[10]; memcpy(B, A, sizeof(A));
    reverse_array(B, n);
    printf("(viii)Reversed = "); print_array(B, n);

    int C[10]; memcpy(C, A, sizeof(A));
    int pivot = 23;
    int idx = partition_ge_first(C, n, pivot);
    printf("(ix)  Partitioned (pivot=%d, >=pivot FIRST) = ", pivot); print_array(C, n);
    printf("      split index (start of '< pivot' block) = %d\n\n", idx);
}

void run_interactive(void) {
    int n;
    printf("Enter N (number of elements): ");
    if (scanf("%d", &n) != 1 || n <= 1) { printf("Invalid N (need N>=2).\n"); return; }
    int *A = malloc(n * sizeof(int));
    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) { printf("A[%d]: ", i+1); if (scanf("%d", &A[i]) != 1) { printf("Invalid.\n"); free(A); return; } }

    printf("\n(i)   Max = %d\n", find_max(A, n));
    int f, s; first_second_largest(A, n, &f, &s);
    printf("(ii)  First largest = %d, Second largest = %d\n", f, s);
    printf("(iii) Mean = %.3f\n", find_mean(A, n));
    printf("(iv)  Median = %.3f\n", find_median(A, n));
    printf("(v)   Std Dev = %.3f\n", find_stddev(A, n));
    printf("(vi)  Mode = %d\n", find_mode(A, n));

    int *uniq = malloc(n * sizeof(int));
    int m = remove_duplicates(A, n, uniq);
    printf("(vii) Unique (sorted) = "); print_array(uniq, m);
    free(uniq);

    int *B = malloc(n * sizeof(int)); memcpy(B, A, n * sizeof(int));
    reverse_array(B, n);
    printf("(viii)Reversed = "); print_array(B, n);
    free(B);

    int pivot;
    printf("Enter a pivot value for partitioning (ix): ");
    if (scanf("%d", &pivot) == 1) {
        int *C = malloc(n * sizeof(int)); memcpy(C, A, n * sizeof(int));
        int idx = partition_ge_first(C, n, pivot);
        printf("(ix)  Partitioned (>=pivot FIRST) = "); print_array(C, n);
        printf("      split index = %d\n", idx);
        free(C);
    }
    free(A);
}

void run_benchmark(void) {
    FILE *fp = fopen("1D_array_operations_and_their_complexities.csv", "w");
    fprintf(fp, "n,max_ms,first2nd_ms,mean_ms,median_ms,stddev_ms,mode_ms,dedup_ms,reverse_ms,partition_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(1);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *A = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) A[i] = rand() % (n * 2);
        clock_t t0, t1;
        double t_max, t_f2, t_mean, t_med, t_std, t_mode, t_dedup, t_rev, t_part;

        t0 = clock(); find_max(A, n); t1 = clock(); t_max = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        int f, sd; t0 = clock(); first_second_largest(A, n, &f, &sd); t1 = clock(); t_f2 = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        t0 = clock(); find_mean(A, n); t1 = clock(); t_mean = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        t0 = clock(); find_median(A, n); t1 = clock(); t_med = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        t0 = clock(); find_stddev(A, n); t1 = clock(); t_std = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        t0 = clock(); find_mode(A, n); t1 = clock(); t_mode = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        int *uniq = malloc(n * sizeof(int));
        t0 = clock(); remove_duplicates(A, n, uniq); t1 = clock(); t_dedup = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        free(uniq);
        int *B = malloc(n * sizeof(int)); memcpy(B, A, n*sizeof(int));
        t0 = clock(); reverse_array(B, n); t1 = clock(); t_rev = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        free(B);
        int *C = malloc(n * sizeof(int)); memcpy(C, A, n*sizeof(int));
        t0 = clock(); partition_ge_first(C, n, A[n/2]); t1 = clock(); t_part = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        free(C);

        printf("n=%8d done\n", n);
        fprintf(fp, "%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                n, t_max, t_f2, t_mean, t_med, t_std, t_mode, t_dedup, t_rev, t_part);
        free(A);
    }
    fclose(fp);
}

int main(void) {
    printf("Q1: 1D array operations and their complexities\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own array\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes 1D_array_operations_and_their_complexities.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}