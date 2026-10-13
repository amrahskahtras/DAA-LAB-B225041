#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static long long *h; static int hn;
static void push(long long x) {
    int i = hn++; h[i] = x;
    while (i > 0) { int p = (i - 1) / 2; if (h[p] > h[i]) { long long t = h[p]; h[p] = h[i]; h[i] = t; i = p; } else break; }
}
static long long pop(void) {
    long long top = h[0]; h[0] = h[--hn]; int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < hn && h[l] < h[m]) m = l;
        if (r < hn && h[r] < h[m]) m = r;
        if (m == i) break;
        long long t = h[i]; h[i] = h[m]; h[m] = t; i = m;
    }
    return top;
}

long long min_cost_connect(long long *L, int n) {
    h = malloc((n + 1) * sizeof(long long)); hn = 0;
    for (int i = 0; i < n; i++) push(L[i]);
    long long cost = 0;
    while (hn > 1) {
        long long a = pop(), b = pop();
        cost += a + b;
        push(a + b);
    }
    free(h);
    return cost;
}

void demo(void) {
    long long a[] = {2, 4, 3}, b[] = {1, 8, 3, 5}, c[] = {5};
    printf("[2,4,3]   -> cost = %lld (expected 14)\n", min_cost_connect(a, 3));
    printf("[1,8,3,5] -> cost = %lld (expected 30)\n", min_cost_connect(b, 4));
    printf("[5]       -> cost = %lld (expected 0)\n\n", min_cost_connect(c, 1));
}

void run_interactive(void) {
    int n;
    printf("Enter number of sticks n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    long long *L = malloc(n * sizeof(long long));
    printf("Enter %d stick lengths:\n", n);
    for (int i = 0; i < n; i++) { printf("L[%d]: ", i + 1); if (scanf("%lld", &L[i]) != 1) { free(L); return; } }
    printf("\nMinimum total cost = %lld\n", min_cost_connect(L, n));
    free(L);
}

void run_benchmark(void) {
    FILE *fp = fopen("Minimum_Cost_to_Connect_Sticks.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(4);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        long long *L = malloc(n * sizeof(long long));
        for (int i = 0; i < n; i++) L[i] = 1 + rand() % 1000;
        clock_t t0 = clock();
        min_cost_connect(L, n);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(L);
    }
    fclose(fp);
}

int main(void) {
    printf("Q4: Minimum Cost to Connect Sticks (greedy + min-heap, O(n log n))\n");
    printf("Choose mode:\n  1 = Enter your own sticks\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Minimum_Cost_to_Connect_Sticks.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}