#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long comparisons;

void selection_sort(int *A, int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            comparisons++;
            if (A[j] < A[min_idx]) min_idx = j;
        }
        int tmp = A[i]; A[i] = A[min_idx]; A[min_idx] = tmp;
    }
}

int is_sorted(int *A, int n) {
    for (int i = 1; i < n; i++) if (A[i - 1] > A[i]) return 0;
    return 1;
}

void fill_random(int *A, int n)   { for (int i = 0; i < n; i++) A[i] = rand() % 1000000; }
void fill_sorted(int *A, int n)   { for (int i = 0; i < n; i++) A[i] = i; }
void fill_reverse(int *A, int n)  { for (int i = 0; i < n; i++) A[i] = n - i; }

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("Use_of_loop_invariants_in_sorting.csv", "w");
    fprintf(csv, "n,random,sorted,reverse,formula_n_n1_2\n");

    int sizes[] = {100, 500, 1000, 2000, 4000, 8000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%6s | %-10s | %-10s | %-10s | %-14s\n", "n", "random", "sorted", "reverse", "n(n-1)/2");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        int *arr = malloc(n * sizeof(int));

        fill_random(arr, n);
        comparisons = 0;
        selection_sort(arr, n);
        long c_rand = comparisons;
        int ok1 = is_sorted(arr, n);

        fill_sorted(arr, n);
        comparisons = 0;
        selection_sort(arr, n);
        long c_sorted = comparisons;
        int ok2 = is_sorted(arr, n);

        fill_reverse(arr, n);
        comparisons = 0;
        selection_sort(arr, n);
        long c_rev = comparisons;
        int ok3 = is_sorted(arr, n);

        long formula = (long) n * (n - 1) / 2;

        printf("%6d | %-10ld | %-10ld | %-10ld | %-14ld  (sorted correctly: %d%d%d)\n",
               n, c_rand, c_sorted, c_rev, formula, ok1, ok2, ok3);
        fprintf(csv, "%d,%ld,%ld,%ld,%ld\n", n, c_rand, c_sorted, c_rev, formula);

        free(arr);
    }

    fclose(csv);
    printf("\nData written to selection_data.csv\n");
    printf("Note: comparisons are IDENTICAL across random/sorted/reverse inputs,\n");
    printf("confirming selection sort's best case is NOT better than its worst case.\n");
    return 0;
}