#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long min_candies(int *r, int n, int *candy_out) {
    int *c = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) c[i] = 1;
    for (int i = 1; i < n; i++) if (r[i] > r[i-1]) c[i] = c[i-1] + 1;
    for (int i = n - 2; i >= 0; i--) if (r[i] > r[i+1] && c[i] < c[i+1] + 1) c[i] = c[i+1] + 1;
    long long sum = 0;
    for (int i = 0; i < n; i++) sum += c[i];
    if (candy_out) for (int i = 0; i < n; i++) candy_out[i] = c[i];
    free(c);
    return sum;
}

void demo(void) {
    int a[] = {1, 0, 2}, b[] = {1, 2, 2}, c[] = {1, 3, 4, 5, 2};
    int out[8];
    long long s = min_candies(a, 3, out);
    printf("[1,0,2]     -> total = %lld (expected 5), candies: %d %d %d\n", s, out[0], out[1], out[2]);
    s = min_candies(b, 3, out);
    printf("[1,2,2]     -> total = %lld (expected 4), candies: %d %d %d\n", s, out[0], out[1], out[2]);
    s = min_candies(c, 5, out);
    printf("[1,3,4,5,2] -> total = %lld (expected 11), candies: %d %d %d %d %d\n\n", s, out[0], out[1], out[2], out[3], out[4]);
}

void run_interactive(void) {
    int n;
    printf("Enter number of children n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *r = malloc(n * sizeof(int)), *c = malloc(n * sizeof(int));
    printf("Enter %d ratings:\n", n);
    for (int i = 0; i < n; i++) { printf("rating[%d]: ", i + 1); if (scanf("%d", &r[i]) != 1) { free(r); free(c); return; } }
    long long s = min_candies(r, n, c);
    printf("\nCandies: "); for (int i = 0; i < n; i++) printf("%d ", c[i]);
    printf("\nMinimum total candies = %lld\n", s);
    free(r); free(c);
}

void run_benchmark(void) {
    FILE *fp = fopen("Candy_Distribution_Problem.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 10000, 100000, 500000, 1000000, 2000000, 5000000, 10000000, 20000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(5);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *r = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) r[i] = rand() % 100000;
        clock_t t0 = clock();
        min_candies(r, n, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%9d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(r);
    }
    fclose(fp);
}

int main(void) {
    printf("Q5: Candy Distribution (two-pass greedy, O(n))\n");
    printf("Choose mode:\n  1 = Enter your own ratings\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Candy_Distribution_Problem.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}