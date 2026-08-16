#include <stdio.h>
#include <stdlib.h>

long comparisons;


int cmp3(int x, int a) {
    comparisons++;
    if (x < a) return -1;
    if (x == a) return 0;
    return 1;
}

int binary_search(int *arr, int lo, int hi, int x) {
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int c = cmp3(x, arr[mid]);
        if (c == 0) return mid;
        else if (c < 0) hi = mid - 1;
        else lo = mid + 1;
    }
    return -1;
}

int ternary_search(int *arr, int lo, int hi, int x) {
    while (lo <= hi) {
        int m1 = lo + (hi - lo) / 3;
        int m2 = hi - (hi - lo) / 3;

        int c1 = cmp3(x, arr[m1]);
        if (c1 == 0) return m1;
        int c2 = cmp3(x, arr[m2]);
        if (c2 == 0) return m2;

        if (c1 < 0)       hi = m1 - 1;
        else if (c2 > 0)  lo = m2 + 1;
        else              { lo = m1 + 1; hi = m2 - 1; }
    }
    return -1;
}

int main(void) {
    FILE *csv = fopen("Binary_vs_Ternary_Search.csv", "w");
    fprintf(csv, "n,binary_comparisons,ternary_comparisons\n");

    int sizes[] = {100, 1000, 10000, 100000, 1000000, 10000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%10s | %-12s | %-12s | %-10s\n", "n", "binary", "ternary", "ratio");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        int *arr = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) arr[i] = 2 * i; 
        int missing = arr[n / 2] + 1; 

        comparisons = 0;
        binary_search(arr, 0, n - 1, missing);
        long c_bin = comparisons;

        comparisons = 0;
        ternary_search(arr, 0, n - 1, missing);
        long c_ter = comparisons;

        printf("%10d | %-12ld | %-12ld | %.3f\n", n, c_bin, c_ter, (double)c_ter/c_bin);
        fprintf(csv, "%d,%ld,%ld\n", n, c_bin, c_ter);

        free(arr);
    }

    fclose(csv);
    printf("\nData written to search_data.csv\n");
    return 0;
}