#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmp_int(const void *a, const void *b) { return (*(int*)a - *(int*)b); }

int binary_search_from(int *arr, int lo_idx, int n, int target) {
    int lo = lo_idx, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int n_global, k_global, T_global;
int *S_global;
int found_flag;
int chosen[64];

void recurse(int depth, int start, long long partial_sum) {
    if (found_flag) return;
    if (depth == k_global - 1) {
        int need = T_global - (int)partial_sum;
        int idx = binary_search_from(S_global, start, n_global, need);
        if (idx != -1) found_flag = 1;
        return;
    }
    for (int i = start; i < n_global && !found_flag; i++) {
        chosen[depth] = i;
        recurse(depth + 1, i + 1, partial_sum + S_global[i]);
    }
}

int k_sum_exists(int *S, int n, int k, int T) {
    int *sorted = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) sorted[i] = S[i];
    qsort(sorted, n, sizeof(int), cmp_int);
    S_global = sorted; n_global = n; k_global = k; T_global = T;
    found_flag = 0;
    recurse(0, 0, 0);
    free(sorted);
    return found_flag;
}

void demo(void) {
    int S[] = {2, 7, 4, 1, 9, 5};
    int T = 15;
    int r = k_sum_exists(S, 6, 3, T);
    printf("Demo: does some k=3 subset sum to %d? %s\n\n", T, r ? "YES" : "NO");
}

void run_interactive(void) {
    int n;
    printf("Enter size n of set S: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    if (n > 63) { printf("Please keep n <= 63 for this demo.\n"); return; }

    int *S = malloc(n * sizeof(int));
    printf("Enter %d integers of S:\n", n);
    for (int i = 0; i < n; i++) {
        printf("S[%d]: ", i + 1);
        if (scanf("%d", &S[i]) != 1) { printf("Invalid input.\n"); free(S); return; }
    }
    int k, T;
    printf("Enter k (how many of them should sum to T): ");
    if (scanf("%d", &k) != 1 || k < 2 || k > n) { printf("Invalid k.\n"); free(S); return; }
    printf("Enter target sum T: ");
    if (scanf("%d", &T) != 1) { printf("Invalid input.\n"); free(S); return; }

    int r = k_sum_exists(S, n, k, T);
    printf("\n%s a subset of %d elements from S sums to %d.\n", r ? "YES: there exists" : "NO: there is no", k, T);

    free(S);
}

int main(void) {
    printf("Q3: Does some k-subset of S sum to T? (O(n^(k-1) log n))\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own S, k, T\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes Application_of_sorting-III.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) { run_interactive(); return 0; }
    else if (choice == 2) { demo(); return 0; }
    else if (choice != 3) { printf("Invalid choice.\n"); return 0; }

    FILE *fp = fopen("Application_of_sorting-III.csv", "w");
    fprintf(fp, "n,k,time_ms\n");

    int k = 4; 
    int sizes[] = {10, 20, 30, 40, 55, 70, 90, 110, 140, 170};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(3);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *S = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) S[i] = rand() % (10 * n);
        int T = -999999; 
        clock_t t0 = clock();
        k_sum_exists(S, n, k, T);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%4d k=%d time=%10.3f ms\n", n, k, ms);
        fprintf(fp, "%d,%d,%.4f\n", n, k, ms);
        free(S);
    }
    fclose(fp);
    return 0;
}