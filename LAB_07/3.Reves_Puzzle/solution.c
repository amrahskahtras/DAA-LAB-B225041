#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

long long frameStewart(int n, int pegs, long long memo[][20]) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    if (pegs == 3) return (1LL << n) - 1;
    if (memo[n][pegs] != -1) return memo[n][pegs];

    long long best = LLONG_MAX;
    for (int k = 1; k < n; k++) {
        long long moves = 2 * frameStewart(k, pegs, memo) + frameStewart(n - k, pegs - 1, memo);
        if (moves < best) best = moves;
    }
    memo[n][pegs] = best;
    return best;
}

int main(int argc, char *argv[]) {
    int maxN = 20;
    int pegs = 4;
    if (argc > 1) maxN = atoi(argv[1]);
    if (argc > 2) pegs = atoi(argv[2]);

    static long long memo[21][20];
    for (int i = 0; i <= maxN; i++)
        for (int j = 0; j < 20; j++)
            memo[i][j] = -1;

    FILE *fp = fopen("data.csv", "w");
    if (!fp) {
        printf("Error opening data.csv\n");
        return 1;
    }

    fprintf(fp, "n_disks,pegs,min_moves\n");
    printf("n_disks\tpegs\tmin_moves\n");

    for (int n = 1; n <= maxN; n++) {
        long long moves = frameStewart(n, pegs, memo);
        fprintf(fp, "%d,%d,%lld\n", n, pegs, moves);
        printf("%d\t%d\t%lld\n", n, pegs, moves);
    }

    printf("\nReve's Puzzle (8 disks, 4 pegs): %lld moves\n", frameStewart(8, 4, memo));

    fclose(fp);
    return 0;
}
