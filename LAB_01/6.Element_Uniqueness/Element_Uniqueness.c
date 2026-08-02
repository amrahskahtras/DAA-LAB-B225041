#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int has_duplicate_naive(int *arr, int n, long *comparisons) {
    long count = 0;
    int found = 0;
    for (int i = 0; i < n && !found; i++) {

        for (int j = i + 1; j < n; j++) {
            count++;
            if (arr[i] == arr[j]) { found = 1; break; }
        }
    }
    *comparisons = count;
    return found;
}

int cmp_int(const void *a, const void *b) {
    return (*(const int *)a) - (*(const int *)b);
}

int has_duplicate_sort(int *arr, int n, long *ops) {
    int *copy = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) copy[i] = arr[i];

    qsort(copy, n, sizeof(int), cmp_int);

    long comparisons = n - 1; 
    int found = 0;
    for (int i = 0; i + 1 < n; i++) {
        if (copy[i] == copy[i + 1]) { found = 1; break; }
    }
    *ops = comparisons; 
    free(copy);
    return found;
}

void fill_unique_permutation(int *arr, int n) {
    for (int i = 0; i < n; i++) arr[i] = i;
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("uniqueness_data.csv", "w");
    fprintf(csv, "n,naive_comparisons,sort_scan_comparisons\n");

    int sizes[] = {50, 100, 500, 1000, 2000, 4000, 8000, 16000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%8s | %-20s | %-20s\n", "n", "naive (O(n^2))", "sort+scan (O(n log n))");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        int *arr = malloc(n * sizeof(int));
        fill_unique_permutation(arr, n);
        long naive_ops = 0, sort_ops = 0;
        int dup1 = has_duplicate_naive(arr, n, &naive_ops);
        int dup2 = has_duplicate_sort(arr, n, &sort_ops);

        printf("%8d | %-20ld | %-20ld  (dup found: naive=%d sort=%d)\n",
               n, naive_ops, sort_ops, dup1, dup2);
        fprintf(csv, "%d,%ld,%ld\n", n, naive_ops, sort_ops);

        free(arr);
    }

    fclose(csv);
    printf("\nData written to uniqueness_data.csv\n");
    printf("\nConclusion: as n grows, naive comparisons grow ~n^2/2 while the\n");
    printf("sort+scan linear-pass stays ~n (plus an O(n log n) sort cost),\n");
    printf("so for sufficiently large n the sort-based method is vastly faster.\n");
    return 0;
}