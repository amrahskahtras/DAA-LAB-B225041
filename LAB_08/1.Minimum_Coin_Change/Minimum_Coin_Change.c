#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define INF INT_MAX

int min_coin_change(int *C, int n, int V, int *coins_used, int *count_used) {
    int *dp = malloc((V + 1) * sizeof(int));
    int *parent_coin = malloc((V + 1) * sizeof(int)); 
    dp[0] = 0; parent_coin[0] = -1;
    for (int v = 1; v <= V; v++) {
        dp[v] = INF; parent_coin[v] = -1;
        for (int i = 0; i < n; i++) {
            if (C[i] <= v && dp[v - C[i]] != INF && dp[v - C[i]] + 1 < dp[v]) {
                dp[v] = dp[v - C[i]] + 1;
                parent_coin[v] = C[i];
            }
        }
    }
    int result = (dp[V] == INF) ? -1 : dp[V];
    if (result != -1 && coins_used) {
        int v = V, cnt = 0;
        while (v > 0) { coins_used[cnt++] = parent_coin[v]; v -= parent_coin[v]; }
        if (count_used) *count_used = cnt;
    } else if (count_used) *count_used = 0;
    free(dp); free(parent_coin);
    return result;
}

void demo(void) {
    int C[] = {1, 5, 6, 8};
    int n = 4, V = 11; 
    int coins_used[64], count_used;
    int res = min_coin_change(C, n, V, coins_used, &count_used);
    printf("Coins: {1,5,6,8}, V=%d -> min coins = %d, used: ", V, res);
    for (int i = 0; i < count_used; i++) printf("%d ", coins_used[i]);
    printf("\n");

    int C2[] = {3, 7};
    int res2 = min_coin_change(C2, 2, 5, NULL, NULL);
    printf("Coins: {3,7}, V=5 -> min coins = %d (should be -1, impossible)\n\n", res2);
}

void run_interactive(void) {
    int n;
    printf("Enter number of coin denominations n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *C = malloc(n * sizeof(int));
    printf("Enter %d coin denominations:\n", n);
    for (int i = 0; i < n; i++) { printf("C[%d]: ", i); if (scanf("%d", &C[i]) != 1) { free(C); return; } }
    int V;
    printf("Enter target amount V: ");
    if (scanf("%d", &V) != 1 || V < 0) { printf("Invalid V.\n"); free(C); return; }

    int *coins_used = malloc((V + 1) * sizeof(int));
    int count_used;
    int res = min_coin_change(C, n, V, coins_used, &count_used);
    if (res == -1) printf("\nAmount %d cannot be made with the given coins.\n", V);
    else {
        printf("\nMinimum coins needed = %d\nCoins used: ", res);
        for (int i = 0; i < count_used; i++) printf("%d ", coins_used[i]);
        printf("\n");
    }
    free(C); free(coins_used);
}

void run_benchmark(void) {
    FILE *fp = fopen("Minimum_Coin_Change.csv", "w");
    fprintf(fp, "V,n_coins,time_ms\n");
    int Vs[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000};
    int nv = sizeof(Vs) / sizeof(Vs[0]);
    int n = 20; 
    int *C = malloc(n * sizeof(int));
    srand(1);
    for (int i = 0; i < n; i++) C[i] = 1 + rand() % 50;
    for (int s = 0; s < nv; s++) {
        int V = Vs[s];
        clock_t t0 = clock();
        min_coin_change(C, n, V, NULL, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("V=%8d  time=%9.3f ms\n", V, ms);
        fprintf(fp, "%d,%d,%.4f\n", V, n, ms);
    }
    free(C);
    fclose(fp);
}

int main(void) {
    printf("Q1: Minimum Coin Change (DP, O(n*V))\n");
    printf("Choose mode:\n  1 = Enter your own coins and V\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Minimum_Coin_Change.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}