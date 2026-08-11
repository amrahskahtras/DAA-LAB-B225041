#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long comparisons;

void merge_two(int *a, int na, int *b, int nb, int *out) {
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) {
        comparisons++;
        if (a[i] <= b[j]) out[k++] = a[i++];
        else               out[k++] = b[j++];
    }
    while (i < na) out[k++] = a[i++];
    while (j < nb) out[k++] = b[j++];
}

int *merge_k_sequential(int **arrays, int k, int n) {
    int *result = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) result[i] = arrays[0][i];
    int result_size = n;

    for (int a = 1; a < k; a++) {
        int *tmp = malloc((result_size + n) * sizeof(int));
        merge_two(result, result_size, arrays[a], n, tmp);
        free(result);
        result = tmp;
        result_size += n;
    }
    return result;
}

int *merge_k_pairwise(int **arrays, int k, int n) {
    int **cur = malloc(k * sizeof(int *));
    int *sizes = malloc(k * sizeof(int));
    for (int i = 0; i < k; i++) {
        cur[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) cur[i][j] = arrays[i][j];
        sizes[i] = n;
    }
    int count = k;

    while (count > 1) {
        int next_count = (count + 1) / 2;
        int **next = malloc(next_count * sizeof(int *));
        int *next_sizes = malloc(next_count * sizeof(int));

        int idx = 0;
        for (int i = 0; i + 1 < count; i += 2) {
            int merged_size = sizes[i] + sizes[i + 1];
            int *merged = malloc(merged_size * sizeof(int));
            merge_two(cur[i], sizes[i], cur[i + 1], sizes[i + 1], merged);
            next[idx] = merged;
            next_sizes[idx] = merged_size;
            idx++;
            free(cur[i]); free(cur[i + 1]);
        }
        if (count % 2 == 1) { 
            next[idx] = cur[count - 1];
            next_sizes[idx] = sizes[count - 1];
            idx++;
        }
        free(cur); free(sizes);
        cur = next; sizes = next_sizes;
        count = next_count;
    }

    int *result = cur[0];
    free(cur); free(sizes);
    return result;
}

void fill_sorted_random(int *arr, int n, int base) {
    arr[0] = base + rand() % 5;
    for (int i = 1; i < n; i++) arr[i] = arr[i - 1] + (rand() % 10);
}

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("kmerge_data.csv", "w");
    fprintf(csv, "k,n,seq_comparisons,pairwise_comparisons\n");

    /* Part A: fix n, vary k */
    int n_fixed = 500;
    int ks[] = {2, 4, 8, 16, 32, 64};
    int num_ks = sizeof(ks) / sizeof(ks[0]);

    printf("=== Fixed n = %d, varying k ===\n", n_fixed);
    printf("%6s | %-18s | %-18s\n", "k", "sequential", "pairwise");
    for (int t = 0; t < num_ks; t++) {
        int k = ks[t];
        int **arrays = malloc(k * sizeof(int *));
        for (int i = 0; i < k; i++) {
            arrays[i] = malloc(n_fixed * sizeof(int));
            fill_sorted_random(arrays[i], n_fixed, 0);
        }

        comparisons = 0;
        int *r1 = merge_k_sequential(arrays, k, n_fixed);
        long c_seq = comparisons;

        comparisons = 0;
        int *r2 = merge_k_pairwise(arrays, k, n_fixed);
        long c_pair = comparisons;

        printf("%6d | %-18ld | %-18ld\n", k, c_seq, c_pair);
        fprintf(csv, "%d,%d,%ld,%ld\n", k, n_fixed, c_seq, c_pair);

        free(r1); free(r2);
        for (int i = 0; i < k; i++) free(arrays[i]);
        free(arrays);
    }

    int k_fixed = 16;
    int ns[] = {100, 200, 400, 800, 1600, 3200, 6400};
    int num_ns = sizeof(ns) / sizeof(ns[0]);

    printf("\n=== Fixed k = %d, varying n ===\n", k_fixed);
    printf("%6s | %-18s | %-18s\n", "n", "sequential", "pairwise");
    for (int t = 0; t < num_ns; t++) {
        int n = ns[t];
        int **arrays = malloc(k_fixed * sizeof(int *));
        for (int i = 0; i < k_fixed; i++) {
            arrays[i] = malloc(n * sizeof(int));
            fill_sorted_random(arrays[i], n, 0);
        }

        comparisons = 0;
        int *r1 = merge_k_sequential(arrays, k_fixed, n);
        long c_seq = comparisons;

        comparisons = 0;
        int *r2 = merge_k_pairwise(arrays, k_fixed, n);
        long c_pair = comparisons;

        printf("%6d | %-18ld | %-18ld\n", n, c_seq, c_pair);
        fprintf(csv, "%d,%d,%ld,%ld\n", k_fixed, n, c_seq, c_pair);

        free(r1); free(r2);
        for (int i = 0; i < k_fixed; i++) free(arrays[i]);
        free(arrays);
    }

    fclose(csv);
    printf("\nData written to kmerge_data.csv\n");
    return 0;
}