#include <stdio.h>
#include <stdlib.h>

long long minMovesAllOff(int n) {
    long long p = 1LL << (n + 1);
    if (n % 2 == 0) {
        return (p - 2) / 3;
    } else {
        return (p - 1) / 3;
    }
}

int main(int argc, char *argv[]) {
    int maxN = 25;
    if (argc > 1) maxN = atoi(argv[1]);

    FILE *fp = fopen("data.csv", "w");
    if (!fp) {
        printf("Error opening data.csv\n");
        return 1;
    }

    fprintf(fp, "n_switches,min_moves\n");
    printf("n_switches\tmin_moves\n");

    for (int n = 1; n <= maxN; n++) {
        long long moves = minMovesAllOff(n);
        fprintf(fp, "%d,%lld\n", n, moves);
        printf("%d\t\t%lld\n", n, moves);
    }

    fclose(fp);
    return 0;
}
