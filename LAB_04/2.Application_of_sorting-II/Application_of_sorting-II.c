#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmp_int(const void *a, const void *b) { return (*(int*)a - *(int*)b); }

int binary_search(int *arr, int n, int target) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int pair_exists(int *S1, int *S2, int n, int x, int *out_a, int *out_b) {
    int *sorted2 = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) sorted2[i] = S2[i];
    qsort(sorted2, n, sizeof(int), cmp_int);
    int found = 0;
    for (int i = 0; i < n; i++) {
        int need = x - S1[i];
        int idx = binary_search(sorted2, n, need);
        if (idx != -1) {
            found = 1;
            if (out_a) *out_a = S1[i];
            if (out_b) *out_b = sorted2[idx];
            break;
        }
    }
    free(sorted2);
    return found;
}

void demo(void) {
    int S1[] = {1, 4, 7, 12, 15};
    int S2[] = {2, 3, 9, 20, 25};
    int x = 21; 
    int a, b;
    int r = pair_exists(S1, S2, 5, x, &a, &b);
    printf("Demo: x=%d -> %s", x, r ? "PAIR FOUND: " : "no pair found\n");
    if (r) printf("%d + %d = %d\n\n", a, b, x); else printf("\n");
}

void run_interactive(void) {
    int n;
    printf("Enter size n of each set (S1 and S2 both have n elements): ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }

    int *S1 = malloc(n * sizeof(int));
    int *S2 = malloc(n * sizeof(int));

    printf("Enter %d elements of S1:\n", n);
    for (int i = 0; i < n; i++) {
        printf("S1[%d]: ", i + 1);
        if (scanf("%d", &S1[i]) != 1) { printf("Invalid input.\n"); free(S1); free(S2); return; }
    }
    printf("Enter %d elements of S2:\n", n);
    for (int i = 0; i < n; i++) {
        printf("S2[%d]: ", i + 1);
        if (scanf("%d", &S2[i]) != 1) { printf("Invalid input.\n"); free(S1); free(S2); return; }
    }
    int x;
    printf("Enter target sum x: ");
    if (scanf("%d", &x) != 1) { printf("Invalid input.\n"); free(S1); free(S2); return; }

    int a, b;
    int found = pair_exists(S1, S2, n, x, &a, &b);
    if (found) printf("\nPAIR FOUND: %d (from S1) + %d (from S2) = %d\n", a, b, x);
    else printf("\nNo pair (one from S1, one from S2) sums to %d.\n", x);

    free(S1); free(S2);
}

int main(void) {
    printf("Q2: Pair summing to x, one element from S1 and one from S2 (O(n log n))\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own S1, S2, x\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes Application_of_sorting-II.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) { run_interactive(); return 0; }
    else if (choice == 2) { demo(); return 0; }
    else if (choice != 3) { printf("Invalid choice.\n"); return 0; }

    FILE *fp = fopen("Application_of_sorting-II.csv", "w");
    fprintf(fp, "n,time_ms\n");

    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(7);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *S1 = malloc(n * sizeof(int));
        int *S2 = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) { S1[i] = rand() % (n * 4); S2[i] = rand() % (n * 4); }
        int x = -1; 
        clock_t t0 = clock();
        pair_exists(S1, S2, n, x, NULL, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(S1); free(S2);
    }
    fclose(fp);
    return 0;
}