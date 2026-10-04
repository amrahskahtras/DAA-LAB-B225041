#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long coin_change_ways(int *C, int n, int V) {
    long long *dp = calloc(V + 1, sizeof(long long));
    dp[0] = 1;
    for (int i = 0; i < n; i++) {
        for (int v = C[i]; v <= V; v++) dp[v] += dp[v - C[i]];
    }
    long long result = dp[V];
    free(dp);
    return result;
}

void demo(void) {
    int C[] = {1, 2, 3};
    int V = 4;
    printf("Coins: {1,2,3}, V=%d -> number of ways = %lld (expected 4)\n", V, coin_change_ways(C, 3, V));

    int C2[] = {2, 5, 3, 6};
    int V2 = 10;
    printf("Coins: {2,5,3,6}, V=%d -> number of ways = %lld\n\n", V2, coin_change_ways(C2, 4, V2));
}

void run_interactive(void) {
    int n;
    printf("Enter number of coin denominations n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *C = malloc(n * sizeof(int));
    printf("Enter %d DISTINCT coin denominations:\n", n);
    for (int i = 0; i < n; i++) { printf("C[%d]: ", i); if (scanf("%d", &C[i]) != 1) { free(C); return; } }
    int V;
    printf("Enter target amount V: ");
    if (scanf("%d", &V) != 1 || V < 0) { printf("Invalid V.\n"); free(C); return; }
    printf("\nNumber of distinct combinations = %lld\n", coin_change_ways(C, n, V));
    free(C);
}

void run_benchmark(void) {
    FILE *fp = fopen("Coin_Change:Total_number_of_ways.csv", "w");
    fprintf(fp, "V,n_coins,time_ms\n");
    int Vs[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000};
    int nv = sizeof(Vs) / sizeof(Vs[0]);
    int n = 20;
    int *C = malloc(n * sizeof(int));
    srand(2);
    for (int i = 0; i < n; i++) C[i] = 1 + rand() % 50;
    for (int s = 0; s < nv; s++) {
        int V = Vs[s];
        clock_t t0 = clock();
        coin_change_ways(C, n, V);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("V=%8d  time=%9.3f ms\n", V, ms);
        fprintf(fp, "%d,%d,%.4f\n", V, n, ms);
    }
    free(C);
    fclose(fp);
}

int main(void) {
    printf("Q2: Coin Change - Total Number of Ways (DP, O(n*V))\n");
    printf("Choose mode:\n  1 = Enter your own coins and V\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Coin_Change:Total_number_of_ways.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
