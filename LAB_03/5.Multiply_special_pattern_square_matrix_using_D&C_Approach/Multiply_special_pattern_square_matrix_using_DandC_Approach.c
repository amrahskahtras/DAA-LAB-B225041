#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long mult_ops; 

long long *multiply_special(long long *a, long long *b, int n) {
    long long *c = malloc(n * sizeof(long long));
    if (n == 1) {
        c[0] = a[0] * b[0];
        mult_ops++;
        return c;
    }
    int h = n / 2;
    long long *a1 = a, *a2 = a + h;
    long long *b1 = b, *b2 = b + h;

    long long *p1 = multiply_special(a1, b1, h); 
    long long *p2 = multiply_special(a2, b2, h); 
    long long *p3 = multiply_special(a1, b2, h); 
    long long *p4 = multiply_special(a2, b1, h); 

    for (int i = 0; i < h; i++) {
        c[i]     = p1[i] + p2[i]; 
        c[h + i] = p3[i] + p4[i]; 
    }

    free(p1); free(p2); free(p3); free(p4);
    return c;
}

long long **expand(long long *gen, int n) {
    long long **M = malloc(n * sizeof(long long *));
    for (int i = 0; i < n; i++) {
        M[i] = malloc(n * sizeof(long long));
        for (int j = 0; j < n; j++) M[i][j] = gen[i ^ j];
    }
    return M;
}
long long **standard_multiply_full(long long **A, long long **B, int n) {
    long long **C = malloc(n * sizeof(long long *));
    for (int i = 0; i < n; i++) {
        C[i] = calloc(n, sizeof(long long));
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];
    }
    return C;
}
void free_full(long long **M, int n) { for (int i=0;i<n;i++) free(M[i]); free(M); }

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("Multiply_special_pattern_square_matrix_using_DandC_Approach.csv", "w");
    fprintf(csv, "n,mult_ops,n_squared\n");

    int sizes[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%6s | %-12s | %-12s | %-8s\n", "n", "mult_ops", "n^2", "correct?");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        long long *a = malloc(n * sizeof(long long));
        long long *b = malloc(n * sizeof(long long));
        for (int i = 0; i < n; i++) { a[i] = rand() % 10; b[i] = rand() % 10; }

        mult_ops = 0;
        long long *c = multiply_special(a, b, n);

        long long **A = expand(a, n), **B = expand(b, n);
        long long **C_full = standard_multiply_full(A, B, n);
        long long **C_from_gen = expand(c, n);

        int correct = 1;
        for (int i = 0; i < n && correct; i++)
            for (int j = 0; j < n; j++)
                if (C_full[i][j] != C_from_gen[i][j]) { correct = 0; break; }

        printf("%6d | %-12ld | %-12d | %s\n", n, mult_ops, n*n, correct ? "yes" : "NO");
        fprintf(csv, "%d,%ld,%d\n", n, mult_ops, n*n);

        free(a); free(b); free(c);
        free_full(A,n); free_full(B,n); free_full(C_full,n); free_full(C_from_gen,n);
    }

    fclose(csv);
    printf("\nData written to special_data.csv\n");
    return 0;
}