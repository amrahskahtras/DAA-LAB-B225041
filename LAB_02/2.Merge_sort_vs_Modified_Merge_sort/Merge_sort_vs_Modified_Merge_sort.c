/*
 * mergesort_compare.c
 *
 * Q2: Merge sort (2-way, halves) vs Modified merge sort (3-way, thirds).
 *
 * Both are implemented to count the number of COMPARISONS made during
 * sorting, so we can empirically confirm both are Theta(n log n),
 * just with different constants (log base 2 vs log base 3).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long comparisons;

/* ---------------- Standard 2-way merge sort ---------------- */
void merge2(int *arr, int lo, int mid, int hi, int *tmp) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        comparisons++;
        if (arr[i] <= arr[j]) tmp[k++] = arr[i++];
        else                  tmp[k++] = arr[j++];
    }
    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= hi)  tmp[k++] = arr[j++];
    for (int x = lo; x <= hi; x++) arr[x] = tmp[x];
}
void mergesort2(int *arr, int lo, int hi, int *tmp) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    mergesort2(arr, lo, mid, tmp);
    mergesort2(arr, mid + 1, hi, tmp);
    merge2(arr, lo, mid, hi, tmp);
}

/* ---------------- Modified 3-way merge sort ---------------- */
void merge3(int *arr, int lo, int m1, int m2, int hi, int *tmp) {
    int i = lo, j = m1 + 1, k = m2 + 1, t = lo;
    while (i <= m1 && j <= m2 && k <= hi) {
        comparisons += 2; /* compare 3 candidates -> 2 comparisons to find min */
        if (arr[i] <= arr[j] && arr[i] <= arr[k])      tmp[t++] = arr[i++];
        else if (arr[j] <= arr[i] && arr[j] <= arr[k]) tmp[t++] = arr[j++];
        else                                            tmp[t++] = arr[k++];
    }
    /* one sub-array exhausted: fall back to standard 2-way merges */
    while (i <= m1 && j <= m2) {
        comparisons++;
        if (arr[i] <= arr[j]) tmp[t++] = arr[i++]; else tmp[t++] = arr[j++];
    }
    while (j <= m2 && k <= hi) {
        comparisons++;
        if (arr[j] <= arr[k]) tmp[t++] = arr[j++]; else tmp[t++] = arr[k++];
    }
    while (i <= m1 && k <= hi) {
        comparisons++;
        if (arr[i] <= arr[k]) tmp[t++] = arr[i++]; else tmp[t++] = arr[k++];
    }
    while (i <= m1) tmp[t++] = arr[i++];
    while (j <= m2) tmp[t++] = arr[j++];
    while (k <= hi) tmp[t++] = arr[k++];
    for (int x = lo; x <= hi; x++) arr[x] = tmp[x];
}
void mergesort3(int *arr, int lo, int hi, int *tmp) {
    if (lo >= hi) return;
    int len = hi - lo + 1;
    if (len < 3) { /* base case: fall back to a trivial 2-way split */
        int mid = lo + (hi - lo) / 2;
        mergesort3(arr, lo, mid, tmp);
        mergesort3(arr, mid + 1, hi, tmp);
        merge2(arr, lo, mid, hi, tmp);
        return;
    }
    int m1 = lo + len / 3 - 1;
    int m2 = lo + 2 * len / 3 - 1;
    mergesort3(arr, lo, m1, tmp);
    mergesort3(arr, m1 + 1, m2, tmp);
    mergesort3(arr, m2 + 1, hi, tmp);
    merge3(arr, lo, m1, m2, hi, tmp);
}

void fill_random(int *arr, int n) {
    for (int i = 0; i < n; i++) arr[i] = rand() % 1000000;
}

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("mergesort_data.csv", "w");
    fprintf(csv, "n,mergesort2_comparisons,mergesort3_comparisons\n");

    int sizes[] = {100, 500, 1000, 5000, 10000, 50000, 100000, 500000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%8s | %-22s | %-22s\n", "n", "2-way (halves)", "3-way (thirds)");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        int *base = malloc(n * sizeof(int));
        int *work = malloc(n * sizeof(int));
        int *tmp  = malloc(n * sizeof(int));
        fill_random(base, n);

        for (int i = 0; i < n; i++) work[i] = base[i];
        comparisons = 0;
        mergesort2(work, 0, n - 1, tmp);
        long c2 = comparisons;

        for (int i = 0; i < n; i++) work[i] = base[i];
        comparisons = 0;
        mergesort3(work, 0, n - 1, tmp);
        long c3 = comparisons;

        printf("%8d | %-22ld | %-22ld\n", n, c2, c3);
        fprintf(csv, "%d,%ld,%ld\n", n, c2, c3);

        free(base); free(work); free(tmp);
    }

    fclose(csv);
    printf("\nData written to mergesort_data.csv\n");
    return 0;
}