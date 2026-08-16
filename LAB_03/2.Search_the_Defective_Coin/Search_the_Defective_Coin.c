#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long weighings;

int weigh(double *coins, int lstart, int lend, int rstart, int rend) {
    weighings++;
    double lsum = 0, rsum = 0;
    for (int i = lstart; i < lend; i++) lsum += coins[i];
    for (int i = rstart; i < rend; i++) rsum += coins[i];
    if (lsum < rsum) return -1;
    if (rsum < lsum) return 1;
    return 0;
}

int find_defective(double *coins, int lo, int hi) {
    int n = hi - lo;
    if (n <= 0) return -1;
    if (n == 1) return lo; 
                               

    int mid = lo + n / 2;
    int result = weigh(coins, lo, mid, mid, hi);

    if (result == 0) {
        return -1; 
    } else if (result == -1) {
        return find_defective(coins, lo, mid);  
    } else {
        return find_defective(coins, mid, hi);  
    }
}

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("Search_the_Defective_Coin.csv", "w");
    fprintf(csv, "n,weighings_with_defect,weighings_no_defect\n");

    int sizes[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 4096, 16384};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%8s | %-22s | %-22s\n", "n", "weighings (defect)", "weighings (no defect)");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        double *coins = malloc(n * sizeof(double));

        for (int i = 0; i < n; i++) coins[i] = 10.0;
        coins[n - 1] = 9.5; 
        weighings = 0;
        int found = find_defective(coins, 0, n);
        long w_defect = weighings;

        for (int i = 0; i < n; i++) coins[i] = 10.0;
        weighings = 0;
        int found2 = find_defective(coins, 0, n);
        long w_nodefect = weighings;

        printf("%8d | %-22ld | %-22ld  (found idx=%d, none-check=%d)\n",
               n, w_defect, w_nodefect, found, found2);
        fprintf(csv, "%d,%ld,%ld\n", n, w_defect, w_nodefect);

        free(coins);
    }

    fclose(csv);
    printf("\nData written to coin_data.csv\n");
    return 0;
}