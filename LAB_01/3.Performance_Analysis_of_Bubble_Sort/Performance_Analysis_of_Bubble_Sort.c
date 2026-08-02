#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long bubble_sort_early_exit(int arr[], int n) {
    long comparisons = 0;
    int swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            comparisons++;
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
                swapped = 1;
            }
        }
        if (!swapped) break;  
    }
    return comparisons;
}

long bubble_sort_full_passes(int arr[], int n) {
    long comparisons = 0;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            comparisons++;
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
      
    }
    return comparisons;
}

void fill_random(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = rand() % 100000;
}

void copy_array(int src[], int dst[], int n) {
    for (int i = 0; i < n; i++) dst[i] = src[i];
}

int main(void) {
    srand((unsigned) time(NULL));

    int sizes[] = {100, 200, 400, 800, 1200, 1600, 2000, 2500, 3000};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

    FILE *fp = fopen("bubble_results.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Could not open bubble_results.csv\n");
        return 1;
    }
    fprintf(fp, "n,comparisons_early_exit,comparisons_full_passes\n");

    printf("%-8s %-22s %-22s\n", "n", "Early-exit comparisons", "Full-pass comparisons");
    for (int s = 0; s < n_sizes; s++) {
        int n = sizes[s];
        int *original = malloc(n * sizeof(int));
        int *a1 = malloc(n * sizeof(int));
        int *a2 = malloc(n * sizeof(int));

        fill_random(original, n);
        copy_array(original, a1, n);
        copy_array(original, a2, n);

        long comp_early = bubble_sort_early_exit(a1, n);
        long comp_full  = bubble_sort_full_passes(a2, n);

        printf("%-8d %-22ld %-22ld\n", n, comp_early, comp_full);
        fprintf(fp, "%d,%ld,%ld\n", n, comp_early, comp_full);

        free(original);
        free(a1);
        free(a2);
    }
    fclose(fp);

    printf("\nResults written to bubble_results.csv\n");
    printf("Observation: on random data, the early-exit version rarely\n");
    printf("saves much (data is almost never already sorted mid-way), so\n");
    printf("both versions are close to n(n-1)/2 comparisons -> O(n^2).\n");
    printf("The early-exit version only pays off when the input is\n");
    printf("already sorted or nearly sorted, giving it a best case of O(n).\n");

    return 0;
}