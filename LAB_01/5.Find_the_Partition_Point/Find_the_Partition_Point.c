#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int find_partition_linear(int *A, int n) {
    for (int i = 0; i < n; i++) {
        if (A[i] == 1) return i;
    }
    return n; 
}

int find_partition_binary(int *A, int n) {
    int lo = 0, hi = n; 
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (A[mid] == 1) {
            hi = mid;       
        } else {
            lo = mid + 1;   
        }
    }
    return lo; 
}

int *build_array(int n, int zeros) {
    int *A = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) A[i] = (i < zeros) ? 0 : 1;
    return A;
}

int main(void) {
    srand((unsigned) time(NULL));

    printf("=== Correctness check ===\n");
    int n = 20, zeros = 13;
    int *A = build_array(n, zeros);
    printf("Array: ");
    for (int i = 0; i < n; i++) printf("%d ", A[i]);
    printf("\n");
    printf("True partition index = %d\n", zeros);
    printf("Linear scan result   = %d\n", find_partition_linear(A, n));
    printf("Binary search result = %d\n", find_partition_binary(A, n));
    free(A);

    FILE *csv = fopen("partition_data.csv", "w");
    fprintf(csv, "n,linear_ops,binary_ops\n");

    printf("\n=== Operation count vs n (partition roughly at middle) ===\n");
    printf("%8s | %-12s | %-12s\n", "n", "linear (n)", "binary (log2 n)");
    int sizes[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
    for (int s = 0; s < 7; s++) {
        int size = sizes[s];
        int mid_zeros = size / 2;
        int *arr = build_array(size, mid_zeros);

        long lin_ops = 0;
        for (int i = 0; i < size; i++) { lin_ops++; if (arr[i] == 1) break; }

        long bin_ops = 0;
        int lo = 0, hi = size;
        while (lo < hi) {
            bin_ops++;
            int mid = lo + (hi - lo) / 2;
            if (arr[mid] == 1) hi = mid; else lo = mid + 1;
        }

        printf("%8d | %-12ld | %-12ld\n", size, lin_ops, bin_ops);
        fprintf(csv, "%d,%ld,%ld\n", size, lin_ops, bin_ops);

        free(arr);
    }
    fclose(csv);
    printf("\nData written to partition_data.csv\n");
    return 0;
}