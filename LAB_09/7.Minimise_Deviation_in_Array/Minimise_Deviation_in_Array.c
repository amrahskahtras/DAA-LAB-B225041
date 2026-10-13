#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static long long *h; static int hn;
static void push(long long x) {
    int i = hn++; h[i] = x;
    while (i > 0) { int p = (i - 1) / 2; if (h[p] < h[i]) { long long t = h[p]; h[p] = h[i]; h[i] = t; i = p; } else break; }
}
static long long pop(void) {
    long long top = h[0]; h[0] = h[--hn]; int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < hn && h[l] > h[m]) m = l;
        if (r < hn && h[r] > h[m]) m = r;
        if (m == i) break;
        long long t = h[i]; h[i] = h[m]; h[m] = t; i = m;
    }
    return top;
}

long long min_deviation(long long *A, int n) {
    h = malloc((n + 1) * sizeof(long long)); hn = 0;
    long long mn = (1LL << 62);
    for (int i = 0; i < n; i++) {
        long long x = (A[i] % 2) ? A[i] * 2 : A[i];
        if (x < mn) mn = x;
        push(x);
    }
    long long best = (1LL << 62);
    while (1) {
        long long mx = pop();
        if (mx - mn < best) best = mx - mn;
        if (mx % 2 != 0) break;
        long long half = mx / 2;
        if (half < mn) mn = half;
        push(half);
    }
    free(h);
    return best;
}

void demo(void) {
    long long a[] = {1, 2, 3, 4}, b[] = {4, 1, 5, 20, 3}, c[] = {2, 10, 8};
    printf("[1,2,3,4]    -> min deviation = %lld (expected 1)\n", min_deviation(a, 4));
    printf("[4,1,5,20,3] -> min deviation = %lld (expected 3)\n", min_deviation(b, 5));
    printf("[2,10,8]     -> min deviation = %lld (expected 3)\n\n", min_deviation(c, 3));
}

void run_interactive(void) {
    int n;
    printf("Enter n: "); if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    long long *A = malloc(n * sizeof(long long));
    printf("Enter %d positive integers:\n", n);
    for (int i = 0; i < n; i++) { printf("A[%d]: ", i + 1); if (scanf("%lld", &A[i]) != 1 || A[i] <= 0) { printf("Invalid.\n"); free(A); return; } }
    printf("\nMinimum deviation = %lld\n", min_deviation(A, n));
    free(A);
}

void run_benchmark(void) {
    FILE *fp = fopen("Minimise_Deviation_in_Array.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(7);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        long long *A = malloc(n * sizeof(long long));
        for (int i = 0; i < n; i++) A[i] = 1 + rand() % 1000000000;
        clock_t t0 = clock();
        min_deviation(A, n);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(A);
    }
    fclose(fp);
}

int main(void) {
    printf("Q7: Minimise Deviation (two-way greedy + max-heap)\n");
    printf("Choose mode:\n  1 = Enter your own array\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Minimise_Deviation_in_Array.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}