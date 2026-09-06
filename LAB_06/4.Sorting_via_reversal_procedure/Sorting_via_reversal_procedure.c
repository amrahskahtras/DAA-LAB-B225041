#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

long long g_reversal_count = 0;
long long g_reversal_cost = 0;

void reverse_range(int *p, int i, int j) {
    g_reversal_count++;
    g_reversal_cost += (j - i + 1);
    while (i < j) { int t = p[i]; p[i] = p[j]; p[j] = t; i++; j--; }
}

int is_sorted(int *p, int n) { for (int i = 1; i < n; i++) if (p[i] < p[i-1]) return 0; return 1; }

void pancake_sort(int *p, int n) {
    for (int end = n - 1; end > 0; end--) {
        int max_idx = 0;
        for (int i = 1; i <= end; i++) if (p[i] > p[max_idx]) max_idx = i;
        if (max_idx == end) continue;              
        if (max_idx != 0) reverse_range(p, 0, max_idx); 
        reverse_range(p, 0, end);               
    }
}

void rotate_by_reversal(int *p, int lo, int mid, int hi) {
    if (lo >= mid || mid >= hi) return;
    reverse_range(p, lo, mid - 1);
    reverse_range(p, mid, hi - 1);
    reverse_range(p, lo, hi - 1);
}

int lower_bound(int *p, int lo, int hi, int value) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (p[mid] < value) lo = mid + 1; else hi = mid;
    }
    return lo;
}

void merge_by_rotation(int *p, int lo, int mid, int hi) {
    int n1 = mid - lo, n2 = hi - mid;
    if (n1 == 0 || n2 == 0) return;
    if (n1 == 1 && n2 == 1) { if (p[lo] > p[mid]) reverse_range(p, lo, mid); return; }

    if (n1 <= n2) {
        int m1 = lo + n1 / 2;                         
        int val = p[m1];
        int m2 = lower_bound(p, mid, hi, val);         
        rotate_by_reversal(p, m1, mid, m2);
        int new_mid = m1 + (m2 - mid);
        merge_by_rotation(p, lo, m1, new_mid);
        merge_by_rotation(p, new_mid + 1, m2, hi);
    } else {
        int m1 = mid + n2 / 2;
        int val = p[m1];
        int m2 = lower_bound(p, lo, mid, val + 1) ; 
        int new_mid = m2 + (m1 - mid) + 1 - 1;
        int placed = m2 + (m1 - mid);
        merge_by_rotation(p, lo, m2, placed);
        merge_by_rotation(p, placed + 1, m1 + 1, hi);
    }
}

void mergesort_by_reversal(int *p, int lo, int hi) {
    if (hi - lo <= 1) return;
    int mid = lo + (hi - lo) / 2;
    mergesort_by_reversal(p, lo, mid);
    mergesort_by_reversal(p, mid, hi);
    merge_by_rotation(p, lo, mid, hi);
}

void print_array(int *p, int n) { for (int i = 0; i < n; i++) printf("%d ", p[i]); printf("\n"); }

void make_random_permutation(int *p, int n) {
    for (int i = 0; i < n; i++) p[i] = i + 1;
    for (int i = n - 1; i > 0; i--) { int j = rand() % (i + 1); int t = p[i]; p[i] = p[j]; p[j] = t; }
}

void demo(void) {
    int base[] = {1, 4, 3, 2, 5};
    int n = 5;

    int p1[5]; memcpy(p1, base, sizeof(base));
    g_reversal_count = 0; g_reversal_cost = 0;
    printf("Input permutation: "); print_array(p1, n);
    pancake_sort(p1, n);
    printf("PART A (pancake selection sort) -> "); print_array(p1, n);
    printf("  reversals used = %lld, total cost = %lld, sorted correctly = %s\n",
           g_reversal_count, g_reversal_cost, is_sorted(p1, n) ? "YES" : "NO");

    int p2[5]; memcpy(p2, base, sizeof(base));
    g_reversal_count = 0; g_reversal_cost = 0;
    mergesort_by_reversal(p2, 0, n);
    printf("PART B (merge sort via rotation) -> "); print_array(p2, n);
    printf("  reversals used = %lld, total cost = %lld, sorted correctly = %s\n\n",
           g_reversal_count, g_reversal_cost, is_sorted(p2, n) ? "YES" : "NO");
}

void run_interactive(void) {
    int n;
    printf("Enter n (permutation of 1..n): ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *p = malloc(n * sizeof(int));
    printf("Enter the permutation (n distinct integers from 1 to %d):\n", n);
    for (int i = 0; i < n; i++) { printf("p[%d]: ", i); if (scanf("%d", &p[i]) != 1) { printf("Invalid.\n"); free(p); return; } }

    int *p1 = malloc(n * sizeof(int)); memcpy(p1, p, n * sizeof(int));
    g_reversal_count = 0; g_reversal_cost = 0;
    pancake_sort(p1, n);
    printf("\nPART A (pancake selection sort) -> "); print_array(p1, n);
    printf("  reversals used = %lld, total cost = %lld, sorted correctly = %s\n",
           g_reversal_count, g_reversal_cost, is_sorted(p1, n) ? "YES" : "NO");
    free(p1);

    int *p2 = malloc(n * sizeof(int)); memcpy(p2, p, n * sizeof(int));
    g_reversal_count = 0; g_reversal_cost = 0;
    mergesort_by_reversal(p2, 0, n);
    printf("PART B (merge sort via rotation) -> "); print_array(p2, n);
    printf("  reversals used = %lld, total cost = %lld, sorted correctly = %s\n",
           g_reversal_count, g_reversal_cost, is_sorted(p2, n) ? "YES" : "NO");
    free(p2);

    free(p);
}

void run_benchmark(void) {
    FILE *fp = fopen("Sorting_via_reversal_procedure.csv", "w");
    fprintf(fp, "n,pancake_count,pancake_cost,mergerev_count,mergerev_cost\n");
    int sizes[] = {100, 200, 500, 1000, 2000, 4000, 8000, 16000, 32000, 64000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(4);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *p = malloc(n * sizeof(int));
        make_random_permutation(p, n);

        int *p1 = malloc(n * sizeof(int)); memcpy(p1, p, n * sizeof(int));
        g_reversal_count = 0; g_reversal_cost = 0;
        pancake_sort(p1, n);
        long long pc_count = g_reversal_count, pc_cost = g_reversal_cost;
        int ok1 = is_sorted(p1, n);
        free(p1);

        int *p2 = malloc(n * sizeof(int)); memcpy(p2, p, n * sizeof(int));
        g_reversal_count = 0; g_reversal_cost = 0;
        mergesort_by_reversal(p2, 0, n);
        long long mr_count = g_reversal_count, mr_cost = g_reversal_cost;
        int ok2 = is_sorted(p2, n);
        free(p2);

        printf("n=%6d  pancake(count=%lld,cost=%lld,ok=%d)  mergerev(count=%lld,cost=%lld,ok=%d)\n",
               n, pc_count, pc_cost, ok1, mr_count, mr_cost, ok2);
        fprintf(fp, "%d,%lld,%lld,%lld,%lld\n", n, pc_count, pc_cost, mr_count, mr_cost);
        free(p);
    }
    fclose(fp);
}

int main(void) {
    printf("Q4: Sorting via reversal -- O(n) reversal count vs O(n log^2 n) reversal cost\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own permutation\n");
    printf("  2 = Run built-in demo (matches the [1,4,3,2,5] example from the lab sheet)\n");
    printf("  3 = Run timing/cost benchmark (writes Sorting_via_reversal_procedure.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    srand((unsigned)time(NULL));
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}