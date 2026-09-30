#include <stdio.h>
#include <stdlib.h>

long long totalCoins(int n) {
    return (long long)n * (n + 1) / 2;
}

long long minMovesToInvert(int n) {
    long long coins = totalCoins(n);
    return coins / 3;
}

int main(int argc, char *argv[]) {
    int maxN = 30;
    if (argc > 1) maxN = atoi(argv[1]);

    FILE *fp = fopen("data.csv", "w");
    if (!fp) {
        printf("Error opening data.csv\n");
        return 1;
    }

    fprintf(fp, "n_rows,total_coins,min_moves\n");
    printf("n_rows\ttotal_coins\tmin_moves\n");

    for (int n = 1; n <= maxN; n++) {
        long long coins = totalCoins(n);
        long long moves = minMovesToInvert(n);
        fprintf(fp, "%d,%lld,%lld\n", n, coins, moves);
        printf("%d\t%lld\t\t%lld\n", n, coins, moves);
    }

    fclose(fp);
    return 0;
}
