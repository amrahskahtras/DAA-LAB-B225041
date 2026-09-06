#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct { double *d; int n; } Mat;

Mat mat_alloc(int n) { Mat m; m.n = n; m.d = calloc((size_t)n * n, sizeof(double)); return m; }
void mat_free(Mat *m) { free(m->d); m->d = NULL; }
#define AT(m, i, j) ((m).d[(size_t)(i) * (m).n + (j)])

Mat mat_add(Mat A, Mat B) {
    int n = A.n;
    Mat C = mat_alloc(n);
    for (int i = 0; i < n * n; i++) C.d[i] = A.d[i] + B.d[i];
    return C;
}

Mat mat_mul(Mat A, Mat B) {
    int n = A.n;
    Mat C = mat_alloc(n);
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++) {
            double a = AT(A, i, k);
            if (a == 0.0) continue;
            for (int j = 0; j < n; j++) AT(C, i, j) += a * AT(B, k, j);
        }
    return C;
}

int is_zero_matrix(Mat A) {
    int n = A.n;
    for (int i = 0; i < n * n; i++) if (A.d[i] != 0.0) return 0;
    return 1;
}

int is_symmetric(Mat A) {
    int n = A.n;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (AT(A, i, j) != AT(A, j, i)) return 0;
    return 1;
}

double mat_determinant(Mat A_in) {
    int n = A_in.n;
    Mat A = mat_alloc(n);
    memcpy(A.d, A_in.d, (size_t)n * n * sizeof(double));
    double det = 1.0;
    for (int col = 0; col < n; col++) {
        int piv = col;
        for (int r = col + 1; r < n; r++) if (fabs(AT(A, r, col)) > fabs(AT(A, piv, col))) piv = r;
        if (fabs(AT(A, piv, col)) < 1e-12) { mat_free(&A); return 0.0; }
        if (piv != col) {
            for (int j = 0; j < n; j++) { double t = AT(A, col, j); AT(A, col, j) = AT(A, piv, j); AT(A, piv, j) = t; }
            det = -det;
        }
        det *= AT(A, col, col);
        for (int r = col + 1; r < n; r++) {
            double factor = AT(A, r, col) / AT(A, col, col);
            for (int j = col; j < n; j++) AT(A, r, j) -= factor * AT(A, col, j);
        }
    }
    mat_free(&A);
    return det;
}

void mat_transpose_inplace(Mat A) {
    int n = A.n;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double t = AT(A, i, j); AT(A, i, j) = AT(A, j, i); AT(A, j, i) = t;
        }
}

double power_iteration(Mat A, double *eigvec, int max_iters, double tol) {
    int n = A.n;
    double *v = malloc(n * sizeof(double));
    double *w = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) v[i] = 1.0 / sqrt((double)n); 
    double lambda_old = 0, lambda = 0;
    for (int it = 0; it < max_iters; it++) {
        for (int i = 0; i < n; i++) {
            double s = 0;
            for (int j = 0; j < n; j++) s += AT(A, i, j) * v[j];
            w[i] = s;
        }
        double norm = 0;
        for (int i = 0; i < n; i++) norm += w[i] * w[i];
        norm = sqrt(norm);
        if (norm < 1e-15) break;
        for (int i = 0; i < n; i++) v[i] = w[i] / norm;
        double num = 0;
        for (int i = 0; i < n; i++) {
            double s = 0;
            for (int j = 0; j < n; j++) s += AT(A, i, j) * v[j];
            num += v[i] * s;
        }
        lambda = num;
        if (fabs(lambda - lambda_old) < tol) { lambda_old = lambda; break; }
        lambda_old = lambda;
    }
    for (int i = 0; i < n; i++) eigvec[i] = v[i];
    free(v); free(w);
    return lambda_old;
}

void print_matrix(Mat A) {
    for (int i = 0; i < A.n; i++) {
        for (int j = 0; j < A.n; j++) printf("%8.2f ", AT(A, i, j));
        printf("\n");
    }
}

void demo(void) {
    int n = 3;
    Mat A = mat_alloc(n), B = mat_alloc(n);
    double a_vals[9] = {2,1,0, 1,3,1, 0,1,2}; 
    double b_vals[9] = {1,0,0, 0,1,0, 0,0,1};
    memcpy(A.d, a_vals, sizeof(a_vals));
    memcpy(B.d, b_vals, sizeof(b_vals));

    printf("A =\n"); print_matrix(A);
    printf("B =\n"); print_matrix(B);

    Mat C = mat_add(A, B);
    printf("(i)   A+B =\n"); print_matrix(C); mat_free(&C);

    Mat D = mat_mul(A, B);
    printf("(ii)  A*B =\n"); print_matrix(D); mat_free(&D);

    printf("(iii) Is A zero matrix? %s\n", is_zero_matrix(A) ? "YES" : "NO");
    printf("(iv)  Is A symmetric?   %s\n", is_symmetric(A) ? "YES" : "NO");
    printf("(v)   det(A) = %.4f\n", mat_determinant(A));

    Mat T = mat_alloc(n); memcpy(T.d, A.d, (size_t)n*n*sizeof(double));
    mat_transpose_inplace(T);
    printf("(vi)  transpose(A) =\n"); print_matrix(T); mat_free(&T);

    double eigvec[3];
    double lambda = power_iteration(A, eigvec, 500, 1e-12);
    printf("(vii) Dominant eigenvalue of A ~= %.4f, eigenvector ~= [%.4f, %.4f, %.4f]\n\n",
           lambda, eigvec[0], eigvec[1], eigvec[2]);

    mat_free(&A); mat_free(&B);
}

void run_interactive(void) {
    int n;
    printf("Enter matrix dimension n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }

    Mat A = mat_alloc(n);
    printf("Enter matrix A (%d x %d), row by row:\n", n, n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        printf("A[%d][%d]: ", i, j);
        if (scanf("%lf", &AT(A, i, j)) != 1) { printf("Invalid.\n"); mat_free(&A); return; }
    }

    Mat B = mat_alloc(n);
    printf("Enter matrix B (%d x %d), row by row (for addition/multiplication):\n", n, n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        printf("B[%d][%d]: ", i, j);
        if (scanf("%lf", &AT(B, i, j)) != 1) { printf("Invalid.\n"); mat_free(&A); mat_free(&B); return; }
    }

    Mat C = mat_add(A, B);
    printf("\n(i)   A+B =\n"); print_matrix(C); mat_free(&C);

    Mat D = mat_mul(A, B);
    printf("(ii)  A*B =\n"); print_matrix(D); mat_free(&D);

    printf("(iii) Is A zero matrix? %s\n", is_zero_matrix(A) ? "YES" : "NO");
    printf("(iv)  Is A symmetric?   %s\n", is_symmetric(A) ? "YES" : "NO");
    printf("(v)   det(A) = %.4f\n", mat_determinant(A));

    Mat T = mat_alloc(n); memcpy(T.d, A.d, (size_t)n*n*sizeof(double));
    mat_transpose_inplace(T);
    printf("(vi)  transpose(A) =\n"); print_matrix(T); mat_free(&T);

    double *eigvec = malloc(n * sizeof(double));
    double lambda = power_iteration(A, eigvec, 1000, 1e-12);
    printf("(vii) Dominant eigenvalue of A ~= %.4f, eigenvector ~= [ ", lambda);
    for (int i = 0; i < n; i++) printf("%.4f ", eigvec[i]);
    printf("]\n");
    free(eigvec);

    mat_free(&A); mat_free(&B);
}

void fill_random(Mat A) {
    for (int i = 0; i < A.n * A.n; i++) A.d[i] = (double)(rand() % 100);
}
void fill_symmetric_random(Mat A) {
    int n = A.n;
    for (int i = 0; i < n; i++) for (int j = i; j < n; j++) {
        double v = (double)(rand() % 100);
        AT(A,i,j) = v; AT(A,j,i) = v;
    }
}

void run_benchmark(void) {
    FILE *fp = fopen("2D_square_matrix_operations_and_their_complexities.csv", "w");
    fprintf(fp, "n,add_ms,mul_ms,zero_ms,sym_ms,det_ms,transpose_ms,eig_ms\n");
    int sizes[] = {10, 20, 40, 60, 90, 130, 180, 240, 300, 380};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(2);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        Mat A = mat_alloc(n), B = mat_alloc(n);
        fill_symmetric_random(A);
        fill_random(B);
        clock_t t0, t1;
        double t_add, t_mul, t_zero, t_sym, t_det, t_tr, t_eig;

        t0 = clock(); Mat C = mat_add(A, B); t1 = clock(); t_add = 1000.0*(t1-t0)/CLOCKS_PER_SEC; mat_free(&C);
        t0 = clock(); Mat D = mat_mul(A, B); t1 = clock(); t_mul = 1000.0*(t1-t0)/CLOCKS_PER_SEC; mat_free(&D);
        t0 = clock(); is_zero_matrix(A); t1 = clock(); t_zero = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        t0 = clock(); is_symmetric(A); t1 = clock(); t_sym = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        t0 = clock(); mat_determinant(A); t1 = clock(); t_det = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        Mat T = mat_alloc(n); memcpy(T.d, A.d, (size_t)n*n*sizeof(double));
        t0 = clock(); mat_transpose_inplace(T); t1 = clock(); t_tr = 1000.0*(t1-t0)/CLOCKS_PER_SEC; mat_free(&T);
        double *eigvec = malloc(n * sizeof(double));
        t0 = clock(); power_iteration(A, eigvec, 200, 1e-10); t1 = clock(); t_eig = 1000.0*(t1-t0)/CLOCKS_PER_SEC;
        free(eigvec);

        printf("n=%4d done\n", n);
        fprintf(fp, "%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n", n, t_add, t_mul, t_zero, t_sym, t_det, t_tr, t_eig);
        mat_free(&A); mat_free(&B);
    }
    fclose(fp);
}

int main(void) {
    printf("Q2: 2D square matrix operations and their complexities\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own matrices\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes 2D_square_matrix_operations_and_their_complexities.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    srand((unsigned)time(NULL));
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}