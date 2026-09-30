#include <stdio.h>
#include <stdlib.h>

int generateShotSequence(int n, int *sequence) {
    int count = 0;
    for (int i = 2; i <= n - 1; i++) {
        sequence[count++] = i;
    }
    for (int i = n - 1; i >= 2; i--) {
        sequence[count++] = i;
    }
    return count;
}

int simulateWorstCase(int n, int *sequence, int shots) {
    for (int startPos = 1; startPos <= n; startPos++) {
        for (int startDir = -1; startDir <= 1; startDir += 2) {
            int pos = startPos;
            int dir = startDir;
            int hit = 0;
            for (int t = 0; t < shots; t++) {
                if (pos == sequence[t]) {
                    hit = 1;
                    break;
                }
                if (pos == 1) dir = 1;
                else if (pos == n) dir = -1;
                pos += dir;
            }
            if (!hit) return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int maxN = 20;
    if (argc > 1) maxN = atoi(argv[1]);

    FILE *fp = fopen("data.csv", "w");
    if (!fp) {
        printf("Error opening data.csv\n");
        return 1;
    }

    fprintf(fp, "n_spots,shots_needed,guaranteed_hit\n");
    printf("n_spots\tshots_needed\tguaranteed_hit\n");

    for (int n = 2; n <= maxN; n++) {
        int *sequence = (int *)malloc(2 * n * sizeof(int));
        int shots = generateShotSequence(n, sequence);
        int guaranteed = simulateWorstCase(n, sequence, shots);
        fprintf(fp, "%d,%d,%d\n", n, shots, guaranteed);
        printf("%d\t%d\t\t%s\n", n, shots, guaranteed ? "YES" : "NO");
        free(sequence);
    }

    fclose(fp);
    return 0;
}
