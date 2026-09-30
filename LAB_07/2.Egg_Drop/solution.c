#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int eggDrop(int E, int F) {
    int dp[E + 1][F + 1];

    for (int f = 0; f <= F; f++) {
        dp[1][f] = f;
    }
    for (int e = 1; e <= E; e++) {
        dp[e][0] = 0;
        dp[e][1] = 1;
    }

    for (int e = 2; e <= E; e++) {
        for (int f = 2; f <= F; f++) {
            dp[e][f] = INT_MAX;
            for (int x = 1; x <= f; x++) {
                int broken = dp[e - 1][x - 1];
                int notBroken = dp[e][f - x];
                int worst = 1 + (broken > notBroken ? broken : notBroken);
                if (worst < dp[e][f]) {
                    dp[e][f] = worst;
                }
            }
        }
    }
    return dp[E][F];
}

int main(int argc, char *argv[]) {
    int maxFloors = 100;
    int maxEggs = 6;
    if (argc > 1) maxFloors = atoi(argv[1]);
    if (argc > 2) maxEggs = atoi(argv[2]);

    FILE *fp = fopen("data.csv", "w");
    if (!fp) {
        printf("Error opening data.csv\n");
        return 1;
    }

    fprintf(fp, "eggs,floors,min_trials\n");
    printf("eggs\tfloors\tmin_trials\n");

    for (int e = 1; e <= maxEggs; e++) {
        for (int f = 10; f <= maxFloors; f += 10) {
            int result = eggDrop(e, f);
            fprintf(fp, "%d,%d,%d\n", e, f, result);
            printf("%d\t%d\t%d\n", e, f, result);
        }
    }

    printf("\nSpecific case: E=2 eggs, F=100 floors -> %d trials\n", eggDrop(2, 100));

    fclose(fp);
    return 0;
}
