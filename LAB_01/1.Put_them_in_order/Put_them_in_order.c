#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_FUNCS 12

typedef struct {
    const char *name;
    double (*fn)(double n);
    int rank;
} Func ;

double f_inv_n(double n){ return 1.0 / n; }
double f_log2n(double n){ return log2(n); }
double f_12sqrtn(double n){ return 12.0 * sqrt(n); }
double f_50n_05(double n){ return 50.0 * pow(n, 0.5); }
double f_n_051(double n){ return pow(n, 0.51); }
double f_2pow32_n(double n){ return pow(2.0, 32.0) * n; }
double f_nlog2n(double n){ return n * log2(n); }
double f_n2_minus_324(double n){ return n * n - 324.0; }
double f_100n2_plus_6n(double n){ return 100.0 * n * n + 6.0 * n; }
double f_2n3(double n){ return 2.0 * n * n * n; }
double f_n_log2n_exp(double n){ return pow(n, log2(n)); }
double f_3pow_n(double n){ return pow(3.0, n); }

int cmp_by_rank(const void *a, const void *b) {
    const Func *fa = (const Func *)a;
    const Func *fb = (const Func *)b;
    return fa->rank - fb->rank;
}

int main(void) {
    Func funcs[NUM_FUNCS] = {
        { "1/n",            f_inv_n,          1  },
        { "log2(n)",        f_log2n,          2  },
        { "12*sqrt(n)",     f_12sqrtn,        3  },
        { "50*n^0.5",       f_50n_05,         4  },
        { "n^0.51",         f_n_051,          5  },
        { "2^32 * n",       f_2pow32_n,       6  },
        { "n*log2(n)",      f_nlog2n,         7  },
        { "n^2 - 324",      f_n2_minus_324,   8  },
        { "100n^2 + 6n",    f_100n2_plus_6n,  9  },
        { "2*n^3",          f_2n3,            10 },
        { "n^(log2 n)",     f_n_log2n_exp,    11 },
        { "3^n",            f_3pow_n,         12 },
    };

    qsort(funcs, NUM_FUNCS, sizeof(Func), cmp_by_rank);

    printf("=== Increasing order of growth (analytical result) ===\n");
    for (int i = 0; i < NUM_FUNCS; i++) {
        printf("%2d. %s\n", i + 1, funcs[i].name);
    }

    FILE *csv = fopen("growth_data.csv", "w");
    if (!csv) { perror("fopen"); return 1; }
    fprintf(csv, "n");
    for (int i = 0; i < NUM_FUNCS; i++) fprintf(csv, ",%s", funcs[i].name);
    fprintf(csv, "\n");

    int sample_n[] = {2, 4, 8, 16, 32, 64, 128, 256, 512};
    int num_n = sizeof(sample_n) / sizeof(sample_n[0]);

    for (int k = 0; k < num_n; k++) {
        int n = sample_n[k];
        fprintf(csv, "%d", n);
        for (int i = 0; i < NUM_FUNCS; i++) {
            double v = funcs[i].fn((double) n);
            fprintf(csv, ",%.6g", v);
        }
        fprintf(csv, "\n");
    }
    fclose(csv);

    printf("\nData written to growth_data.csv (n = 2,4,8,...,1024)\n");
    return 0;
}
