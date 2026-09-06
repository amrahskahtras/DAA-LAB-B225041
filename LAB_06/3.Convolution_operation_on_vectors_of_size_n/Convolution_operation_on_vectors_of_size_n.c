#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct { double re, im; } Cplx;

Cplx c_add(Cplx a, Cplx b) { Cplx r = {a.re+b.re, a.im+b.im}; return r; }
Cplx c_sub(Cplx a, Cplx b) { Cplx r = {a.re-b.re, a.im-b.im}; return r; }
Cplx c_mul(Cplx a, Cplx b) { Cplx r = {a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re}; return r; }

void fft(Cplx *a, int n, int invert) {
    if (n == 1) return;
    int half = n / 2;
    Cplx *even = malloc(half * sizeof(Cplx));
    Cplx *odd  = malloc(half * sizeof(Cplx));
    for (int i = 0; i < half; i++) { even[i] = a[2*i]; odd[i] = a[2*i+1]; }

    fft(even, half, invert);
    fft(odd, half, invert);

    double angle = 2 * M_PI / n * (invert ? -1 : 1);
    Cplx w = {1, 0};
    Cplx wn = {cos(angle), sin(angle)};
    for (int k = 0; k < half; k++) {
        Cplx t = c_mul(w, odd[k]);
        a[k]        = c_add(even[k], t);
        a[k + half] = c_sub(even[k], t);
        w = c_mul(w, wn);
    }
    free(even); free(odd);
}

int next_pow2(int x) { int p = 1; while (p < x) p <<= 1; return p; }

double *convolve_fft(double *A, int m, double *B, int n, int *out_len) {
    int result_len = m + n - 1;
    int N = next_pow2(result_len);

    Cplx *fa = malloc(N * sizeof(Cplx));
    Cplx *fb = malloc(N * sizeof(Cplx));
    for (int i = 0; i < N; i++) {
        fa[i].re = (i < m) ? A[i] : 0.0; fa[i].im = 0.0;
        fb[i].re = (i < n) ? B[i] : 0.0; fb[i].im = 0.0;
    }

    fft(fa, N, 0);
    fft(fb, N, 0);
    for (int i = 0; i < N; i++) fa[i] = c_mul(fa[i], fb[i]); 
    fft(fa, N, 1); 

    double *C = malloc(result_len * sizeof(double));
    for (int i = 0; i < result_len; i++) C[i] = fa[i].re / N; 

    free(fa); free(fb);
    *out_len = result_len;
    return C;
}

double *convolve_naive(double *A, int m, double *B, int n, int *out_len) {
    int result_len = m + n - 1;
    double *C = calloc(result_len, sizeof(double));
    for (int j = 0; j < m; j++)
        for (int k = j; k < j + n; k++)
            C[k] += A[j] * B[k - j];
    *out_len = result_len;
    return C;
}

void print_vec(double *v, int n) { for (int i = 0; i < n; i++) printf("%.2f ", v[i]); printf("\n"); }

void demo(void) {
    double A[] = {1, 2, 3};      
    double B[] = {4, 5, 6, 7};   
    int m = 3, n = 4, len1, len2;

    printf("A = "); print_vec(A, m);
    printf("B = "); print_vec(B, n);

    double *c_naive = convolve_naive(A, m, B, n, &len1);
    printf("Naive O(n*m) convolution:   "); print_vec(c_naive, len1);

    double *c_fft = convolve_fft(A, m, B, n, &len2);
    printf("FFT O(n log n) convolution: "); print_vec(c_fft, len2);

    free(c_naive); free(c_fft);
    printf("\n");
}

void run_interactive(void) {
    int m, n;
    printf("Enter length m of vector A: ");
    if (scanf("%d", &m) != 1 || m <= 0) { printf("Invalid m.\n"); return; }
    double *A = malloc(m * sizeof(double));
    printf("Enter %d values of A:\n", m);
    for (int i = 0; i < m; i++) { printf("A[%d]: ", i); if (scanf("%lf", &A[i]) != 1) { printf("Invalid.\n"); free(A); return; } }

    printf("Enter length n of vector B (must have n >= m): ");
    if (scanf("%d", &n) != 1 || n < m) { printf("Invalid n (need n>=m).\n"); free(A); return; }
    double *B = malloc(n * sizeof(double));
    printf("Enter %d values of B:\n", n);
    for (int i = 0; i < n; i++) { printf("B[%d]: ", i); if (scanf("%lf", &B[i]) != 1) { printf("Invalid.\n"); free(A); free(B); return; } }

    int len;
    double *C = convolve_fft(A, m, B, n, &len);
    printf("\nConvolution C (length %d):\n", len);
    print_vec(C, len);

    free(A); free(B); free(C);
}

void run_benchmark(void) {
    FILE *fp = fopen("Convolution_operation_on_vectors_of_size_n.csv", "w");
    fprintf(fp, "n,fft_ms,naive_ms\n");
    int sizes[] = {100, 200, 500, 1000, 2000, 4000, 8000, 16000, 32000, 64000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(3);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s], m = n; 
        double *A = malloc(m * sizeof(double));
        double *B = malloc(n * sizeof(double));
        for (int i = 0; i < m; i++) A[i] = rand() % 100;
        for (int i = 0; i < n; i++) B[i] = rand() % 100;

        int len;
        clock_t t0 = clock();
        double *Cf = convolve_fft(A, m, B, n, &len);
        clock_t t1 = clock();
        double t_fft = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        free(Cf);

        double t_naive = -1;
        if (n <= 16000) { 
            t0 = clock();
            double *Cn = convolve_naive(A, m, B, n, &len);
            t1 = clock();
            t_naive = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
            free(Cn);
        }

        printf("n=%6d  fft=%9.3f ms  naive=%s\n", n, t_fft, t_naive < 0 ? "skipped" : "");
        if (t_naive >= 0) fprintf(fp, "%d,%.4f,%.4f\n", n, t_fft, t_naive);
        else fprintf(fp, "%d,%.4f,\n", n, t_fft);
        free(A); free(B);
    }
    fclose(fp);
}

int main(void) {
    printf("Q3: Convolution via FFT divide-and-conquer, O(n log n)\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own vectors A, B\n");
    printf("  2 = Run built-in demo (cross-checks against naive convolution)\n");
    printf("  3 = Run timing benchmark (writes Convolution_operation_on_vectors_of_size_n.csv, FFT vs naive)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}