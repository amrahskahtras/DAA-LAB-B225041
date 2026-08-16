#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long comparisons;

typedef struct { int mx, mn; } MaxMin;

MaxMin maxmin_paired(int *arr, int n) {
    MaxMin result;
    int i = 0;

    if (n % 2 == 1) {
        result.mx = result.mn = arr[0];
        i = 1;
    } else {
        comparisons++;
        if (arr[0] > arr[1]) { result.mx = arr[0]; result.mn = arr[1]; }
        else                  { result.mx = arr[1]; result.mn = arr[0]; }
        i = 2;
    }


    for (; i < n; i += 2) {
        int a = arr[i], b = arr[i + 1];
        int local_max, local_min;
        comparisons++;
        if (a > b) { local_max = a; local_min = b; }
        else        { local_max = b; local_min = a; }

        comparisons++;
        if (local_max > result.mx) result.mx = local_max;
        comparisons++;
        if (local_min < result.mn) result.mn = local_min;
    }
    return result;
}


MaxMin maxmin_naive(int *arr, int n) {
    MaxMin result;
    result.mx = result.mn = arr[0];
    for (int i = 1; i < n; i++) {
        comparisons++;
        if (arr[i] > result.mx) result.mx = arr[i];
        comparisons++;
        if (arr[i] < result.mn) result.mn = arr[i];
    }
    return result;
}

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("Max_and_Min_using_DandC_Approach.csv", "w");
    fprintf(csv, "n,dc_comparisons,naive_comparisons,bound_3n_2\n");

    int sizes[] = {16, 64, 99, 101, 256, 999, 1001, 1024, 4096, 12345,
                    16384, 65536, 262144, 1000000, 1048576};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%10s | %-12s | %-12s | %-12s | %-8s\n", "n", "D&C(paired)", "naive", "3n/2 bound", "within?");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        int *arr = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) arr[i] = rand();

        comparisons = 0;
        MaxMin r1 = maxmin_paired(arr, n);
        long c_dc = comparisons;

        comparisons = 0;
        MaxMin r2 = maxmin_naive(arr, n);
        long c_naive = comparisons;

        double bound = 1.5 * n;
        const char *within = (c_dc <= bound + 1e-9) ? "yes" : "NO!!";

        printf("%10d | %-12ld | %-12ld | %-12.1f | %-8s (dc: max=%d min=%d | naive: max=%d min=%d)\n",
               n, c_dc, c_naive, bound, within, r1.mx, r1.mn, r2.mx, r2.mn);
        fprintf(csv, "%d,%ld,%ld,%.1f\n", n, c_dc, c_naive, bound);

        free(arr);
    }

    fclose(csv);
    printf("\nData written to maxmin_data.csv\n");
    return 0;
}