/*
 * kmerge.c
 *
 * Q3: Merging k sorted arrays, each of size n.
 *
 *  Method 1 (sequential): repeatedly merge the running result with the
 *      next array: merge(A1,A2), then merge(result,A3), ..., merge(result,Ak).
 *      Worst-case running time: Theta(k^2 * n)
 *
 *  Method 2 (pairwise / tournament): merge arrays in pairs, then merge
 *      those results in pairs, and so on (like the merge phase of merge
 *      sort applied across k arrays).
 *      Worst-case running time: Theta(k * n * log2(k))
 *
 * We count comparisons made by each method for various (k, n) and dump
 * to CSV.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long comparisons;

/* merge two sorted int arrays a[0..na-1], b[0..nb-1] into out (size na+nb) */
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

/* ---------------- Method 1: sequential merging ---------------- */
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

/* ---------------- Method 2: pairwise (tournament) merging ---------------- */
int *merge_k_pairwise(int **arrays, int k, int n) {
    /* copy pointers/sizes into working lists we can shrink round by round */
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
        if (count % 2 == 1) { /* odd one out, carries over unmerged */
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

    fclose(csv);
    printf("\nData written to kmerge_data.csv\n");
    return 0;
}